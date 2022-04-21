#include "VirtualDisk.h"

#include "Error.h"
#include "Handle.h"
#include "IoCtl.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <ntddscsi.h>
#define _NTSCSI_USER_MODE_
#include <scsi.h>
#include <Rpc.h>

#include <string>
#include <ranges>
#include <algorithm>
#include <cctype>

namespace FastSpd
{
    static std::string GetDevicePath()
    {
        static constexpr std::string_view DeviceName = "root\\winspd";
        static constexpr std::string_view GLOBALROOT = "\\\\?\\GLOBALROOT";

        using DiHandle = Handle<SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE>;

        DiHandle Handle = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

        if (!Handle)
        {
            throw CreateErrorWin32(GetLastError());
        }

        SP_DEVINFO_DATA Info{ .cbSize = sizeof(Info) };
        for (DWORD Idx = 0; SetupDiEnumDeviceInfo(Handle, Idx, &Info); ++Idx)
        {
            BYTE HwidBuf[256]{};
            if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_HARDWAREID, NULL, HwidBuf, sizeof(HwidBuf) - 2, NULL))
            {
                continue;
            }

            PCSTR HwidBufPtr = (PCHAR)HwidBuf;
            for (std::string_view Id(HwidBufPtr); !Id.empty(); HwidBufPtr += Id.size() + 1, Id = HwidBufPtr)
            {
                static constexpr auto to_lower{ std::ranges::views::transform([](char c) { return std::tolower(c); })};
                if (std::ranges::equal(Id | to_lower, DeviceName))
                {
                    DWORD NameSize;
                    SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, NULL, 0, &NameSize);
                    std::string Ret(GLOBALROOT.size() + NameSize, '\0');
                    GLOBALROOT.copy(Ret.data(), GLOBALROOT.size());
                    if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (PBYTE)Ret.data() + GLOBALROOT.size(), NameSize, NULL))
                    {
                        throw CreateErrorWin32(GetLastError());
                    }
                    return Ret;
                }
            };
        }

        throw CreateErrorWin32(GetLastError());
    }

    static void Provision(HANDLE DeviceHandle, const StorageUnitParams& Params, uint32_t& Btl)
    {
        auto Call = CreateIoParams<'p'>({ .StorageUnitParams = Params });

        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, NULL))
        {
            throw CreateErrorWin32(GetLastError());
        }

        Btl = Call.Return.Btl;
    }

    static void Unprovision(HANDLE DeviceHandle, const GUID& Guid)
    {
        auto Call = CreateIoParams<'u'>({ .Guid = Guid });

        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, NULL))
        {
            throw CreateErrorWin32(GetLastError());
        }
    }

    static void Transact(HANDLE DeviceHandle, IoCall<'t'>& Call, LPOVERLAPPED Overlapped)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, Overlapped))
        {
            DWORD Error = GetLastError();
            if (Error == ERROR_IO_PENDING)
            {
                DWORD BytesTransferred;
                if (!GetOverlappedResult(DeviceHandle, Overlapped, &BytesTransferred, TRUE))
                {
                    throw CreateErrorWin32(GetLastError());
                }
            }
            else
            {
                throw CreateErrorWin32(GetLastError());
            }
        }
    }

    constexpr char ProductId[sizeof(StorageUnitParams::ProductId) + 1] = "L4";
    constexpr char ProductRevisionLevel[sizeof(StorageUnitParams::ProductRevisionLevel) + 1] = "3.9";

    VirtualDisk::VirtualDisk(uint64_t BlockCount, uint32_t BlockSize)
    {
        auto Path = GetDevicePath();

        DeviceHandle = CreateFileA(Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        if (DeviceHandle == INVALID_HANDLE_VALUE)
        {
            throw CreateErrorWin32(GetLastError());
        }

        StorageUnitParams Params{
            .BlockCount = BlockCount,
            .BlockLength = BlockSize,
            .DeviceType = 0,
            .WriteProtected = 1,
            .CacheSupported = 0,
            .UnmapSupported = 0,
            .EjectDisabled = 1,
            .MaxTransferLength = 1 << 21 // 2 MB
        };
        memcpy(Params.ProductId, ProductId, sizeof(Params.ProductId));
        memcpy(Params.ProductRevisionLevel, ProductRevisionLevel, sizeof(Params.ProductRevisionLevel));
        UuidCreate(&Params.Guid);

        Provision(DeviceHandle, Params, Btl);

        Guid = Params.Guid;
        MaxTransferLength = Params.MaxTransferLength;
    }

    VirtualDisk::~VirtualDisk()
    {
        Stop();
        CloseHandle(DeviceHandle);
        DeviceHandle = INVALID_HANDLE_VALUE;
    }

    static DWORD WINAPI SpdStorageUnitDispatcherThread(PVOID Arg)
    {
        ((VirtualDisk*)Arg)->ThreadFunc();
        return 0;
    }

    void VirtualDisk::Start()
    {
        ThreadFlag.clear();
        std::generate_n(std::back_inserter(Threads), 8, [this]() { return std::async(std::launch::async, &VirtualDisk::ThreadFunc2, this); });
    }

    void VirtualDisk::Stop()
    {
        try
        {
            ThreadFlag.test_and_set();
            Unprovision(DeviceHandle, Guid);
            Threads.clear();
        }
        catch (std::system_error e)
        {
            printf("%x %s\n", e.code().value(), e.what());
        }
        catch (std::exception e)
        {
            printf("%s\n", e.what());
        }
    }

    void VirtualDisk::ThreadFunc2()
    {
        try{
        std::unique_ptr<char[]> DataBuffer = std::make_unique<char[]>(MaxTransferLength);

        OVERLAPPED Overlapped{
            .hEvent = CreateEventA(NULL, TRUE, TRUE, NULL)
        };

        auto Call = CreateIoCall<'t'>();
        Call.Btl = Btl;
        Call.DataBuffer = (UINT64)(UINT_PTR)DataBuffer.get();
        Call.IsRequestValid = true;
        Call.IsResponseValid = false;

        while (!ThreadFlag.test())
        {
            if (!ResetEvent(Overlapped.hEvent))
            {
                throw CreateErrorWin32(GetLastError());
            }

            Transact(DeviceHandle, Call, &Overlapped);

            if (Call.Request.Hint == 0)
            {
                Call.IsResponseValid = false;
                continue;
            }

            Call.IsResponseValid = true;
            Call.Response.Status = {};
            switch (Call.Request.Kind)
            {
                case TransactKind::Read:
                    Read(DataBuffer.get(), Call.Request.Op.Read.BlockAddress, Call.Request.Op.Read.BlockCount);
                    break;
                case TransactKind::Write:
                    //Write(DataBuffer.get(), Request.Op.Write.BlockAddress, Request.Op.Write.BlockCount);
                    break;
                case TransactKind::Flush:
                    //Flush(Request.Op.Flush.BlockAddress, Request.Op.Flush.BlockCount);
                    break;
                case TransactKind::Unmap:
                    std::for_each_n((UnmapDescriptor*)DataBuffer.get(), Call.Request.Op.Unmap.Count, [this](const UnmapDescriptor& Descriptor)
                    {
                        //Unmap(Descriptor.BlockAddress, Descriptor.BlockCount);
                    });
                    break;
                default:
                    // SpdStorageUnitStatusSetSense
                    Call.Response.Status = {
                        .ScsiStatus = SCSISTAT_CHECK_CONDITION,
                        .SenseKey = SCSI_SENSE_ILLEGAL_REQUEST,
                        .ASC = SCSI_ADSENSE_ILLEGAL_COMMAND
                    };
                    break;
            }
        }
        }
        catch (std::system_error e)
        {
            printf("%x %s\n", e.code().value(), e.what());
        }
        catch (std::exception e)
        {
            printf("%s\n", e.what());
        }
    }
}