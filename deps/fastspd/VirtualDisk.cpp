#include "VirtualDisk.h"

#include "Error.h"
#include "Handle.h"
#include "IoCtl.h"
#include "Random.h"

// clang-format off
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <setupapi.h>
#include <winioctl.h>
#include <ntddscsi.h>
#define _NTSCSI_USER_MODE_ // NOLINT(bugprone-reserved-identifier, cert-dcl37-c, cert-dcl51-cpp)
#include <scsi.h>
// clang-format on

#include <cctype>
#include <algorithm>
#include <ranges>

namespace FastSpd
{
    // NOLINTBEGIN(modernize-use-nullptr)
    static std::string GetDevicePath()
    {
        static constexpr std::string_view DeviceName = "root\\winspd";
        static constexpr std::string_view GLOBALROOT = "\\\\?\\GLOBALROOT";

        using DiHandle = Handle<SetupDiDestroyDeviceInfoList, HDEVINFO>;

        DiHandle Handle = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

        if (Handle.get() == INVALID_HANDLE_VALUE)
        {
            throw CreateErrorWin32(GetLastError());
        }

        SP_DEVINFO_DATA Info { .cbSize = sizeof(Info) };
        for (DWORD Idx = 0; SetupDiEnumDeviceInfo(Handle, Idx, &Info); ++Idx)
        {
            std::array<BYTE, 256> HwidBuf {};
            if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_HARDWAREID, NULL, HwidBuf.data(), HwidBuf.size() - 2, NULL))
            {
                continue;
            }

            PCSTR HwidBufPtr = reinterpret_cast<PCHAR>(HwidBuf.data());
            for (std::string_view Id(HwidBufPtr); !Id.empty(); HwidBufPtr += Id.size() + 1, Id = HwidBufPtr)
            {
                static constexpr auto to_lower = std::ranges::views::transform([](char c) { return std::tolower(c); });
                if (std::ranges::equal(Id | to_lower, DeviceName))
                {
                    DWORD NameSize = 0;
                    SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, NULL, 0, &NameSize);
                    std::string Ret(GLOBALROOT.size() + NameSize, '\0');
                    GLOBALROOT.copy(Ret.data(), GLOBALROOT.size());
                    if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, reinterpret_cast<PBYTE>(Ret.data()) + GLOBALROOT.size(), NameSize, NULL))
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

        OVERLAPPED Overlapped {
            .hEvent = CreateEventA(NULL, TRUE, TRUE, NULL)
        };
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, &Overlapped))
        {
            DWORD Error = GetLastError();
            if (Error == ERROR_IO_PENDING)
            {
                DWORD BytesTransferred = 0;
                if (!GetOverlappedResult(DeviceHandle, &Overlapped, &BytesTransferred, TRUE))
                {
                    throw CreateErrorWin32(GetLastError());
                }
            }
            else
            {
                throw CreateErrorWin32(Error);
            }
        }
    }

    static void List(HANDLE DeviceHandle)
    {
        union
        {
            IoList Call = CreateIoCall<'l'>();
            std::array<UINT32, 256> Bitmap;
        };

        DWORD BytesTransferred = 0;
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Bitmap, sizeof(Bitmap), &BytesTransferred, NULL))
        {
            throw CreateErrorWin32(GetLastError());
        }

        for (ULONG Idx = 0; Idx < BytesTransferred / sizeof(UINT32); ++Idx)
        {
            UINT32 Btl = Bitmap[Idx];
            printf("%d %d %d\n", (((Btl) >> 16) & 0xff), (((Btl) >> 8) & 0xff), ((Btl)&0xff));
        }
    }

    static void Transact(HANDLE DeviceHandle, IoTransact& Call, LPOVERLAPPED Overlapped)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, Overlapped))
        {
            DWORD Error = GetLastError();
            if (Error == ERROR_IO_PENDING)
            {
                DWORD BytesTransferred = 0;
                if (!GetOverlappedResult(DeviceHandle, Overlapped, &BytesTransferred, TRUE))
                {
                    throw CreateErrorWin32(GetLastError());
                }
            }
            else
            {
                throw CreateErrorWin32(Error);
            }
        }
    }

    struct OverlappedEx
    {
        OVERLAPPED Overlapped;
        IoTransact Call;
    };
    static_assert(sizeof(OverlappedEx) == sizeof(OverlappedExPublic));
    static_assert(sizeof(OVERLAPPED) == sizeof(OverlappedExPublic::Overlapped));
    static_assert(sizeof(IoTransact) == sizeof(OverlappedExPublic::Call));

    static void DispatchTransact(HANDLE DeviceHandle, OverlappedEx& OvlEx)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &OvlEx.Call, sizeof(OvlEx.Call), &OvlEx.Call, sizeof(OvlEx.Call), NULL, &OvlEx.Overlapped))
        {
            DWORD Error = GetLastError();
            if (Error != ERROR_IO_PENDING)
            {
                throw CreateErrorWin32(Error);
            }
        }
    }

    static DWORD Transact(HANDLE DeviceHandle, HANDLE IocpHandle, OverlappedEx** OvlEx)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &(*OvlEx)->Call, sizeof(IoTransact), &(*OvlEx)->Call, sizeof(IoTransact), NULL, &(*OvlEx)->Overlapped))
        {
            DWORD Error = GetLastError();
            if (Error != ERROR_IO_PENDING)
            {
                throw CreateErrorWin32(GetLastError());
            }
        }

        DWORD BytesTransferred = 0;
        ULONG_PTR CompletionKey = 0;
        BOOL Result = FALSE;
        DWORD Error = 0;
        do
        {
            Result = GetQueuedCompletionStatus(IocpHandle, &BytesTransferred, &CompletionKey, reinterpret_cast<LPOVERLAPPED*>(OvlEx), INFINITE);
            Error = GetLastError();
        } while (!Result && !*OvlEx && Error != ERROR_ABANDONED_WAIT_0 && Error != ERROR_INVALID_HANDLE);

        if (Result)
        {
            if (CompletionKey != 0)
            {
                return ERROR_ABANDONED_WAIT_0;
            }
            return ERROR_SUCCESS;
        }

        if (Error == ERROR_INVALID_HANDLE)
        {
            return ERROR_ABANDONED_WAIT_0;
        }
        return Error;
    }

    VirtualDisk::VirtualDisk(uint64_t BlockCount, uint32_t BlockSize) :
        Btl(),
        Guid(),
        IocpHandle(),
        IocpRange()
    {
        auto Path = GetDevicePath();

        DeviceHandle = CreateFileA(Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        if (DeviceHandle == INVALID_HANDLE_VALUE)
        {
            throw CreateErrorWin32(GetLastError());
        }

        StorageUnitParams Params {
            .Guid = UuidCreate(),
            .BlockCount = BlockCount,
            .BlockLength = BlockSize,
            .ProductId = { "L4" },
            .ProductRevisionLevel = { "0.1" },
            .DeviceType = 0,
            .WriteProtected = 1,
            .CacheSupported = 1,
            .UnmapSupported = 1,
            .EjectDisabled = 1,
            .MaxTransferLength = MaxTransferLength
        };

        Provision(DeviceHandle, Params, Btl);

        Guid = Params.Guid;
    }

    // NOLINTNEXTLINE(bugprone-exception-escape)
    VirtualDisk::~VirtualDisk()
    {
        Stop();
        CloseHandle(DeviceHandle);
    }

    void VirtualDisk::Start()
    {
        IocpHandle = CreateIoCompletionPort(DeviceHandle, NULL, NULL, ThreadCount);
        if (IocpHandle == NULL)
        {
            throw CreateErrorWin32(GetLastError());
        }
        if (!SetFileCompletionNotificationModes(DeviceHandle, FILE_SKIP_SET_EVENT_ON_HANDLE))
        {
            throw CreateErrorWin32(GetLastError());
        }

        DataBuffer = std::make_unique<std::array<std::array<std::byte, MaxTransferLength>, CallCount>>();
        for (size_t Idx = 0; Idx < CallCount; ++Idx)
        {
            Threads[Idx] = std::thread(&VirtualDisk::ThreadFunc, this, Idx);
        }
    }

    void VirtualDisk::Stop()
    {
        OverlappedEx Temp {};
        for (auto& Thread : Threads)
        {
            PostQueuedCompletionStatus(IocpHandle, 0, 1, &Temp.Overlapped);
        }
        Unprovision(DeviceHandle, Guid);
        std::ranges::for_each(Threads, &std::thread::join);
        CloseHandle(IocpHandle);
    }

    void VirtualDisk::List()
    {
        FastSpd::List(DeviceHandle);
    }

    void VirtualDisk::ThreadFunc(size_t Idx)
    {
        auto& ThisOvlEx = reinterpret_cast<OverlappedEx&>(IocpRange[Idx]);
        auto& ThisCall = ThisOvlEx.Call;
        ThisCall = CreateIoCall<'t'>();
        ThisCall.Btl = Btl;
        ThisCall.DataBuffer = DataBuffer->at(Idx).data();
        ThisCall.IsRequestValid = true;
        ThisCall.IsResponseValid = false;

        try
        {
            for (auto* OvlEx = &ThisOvlEx;;)
            {
                DWORD Ret = Transact(DeviceHandle, IocpHandle, &OvlEx);
                if (Ret == ERROR_ABANDONED_WAIT_0)
                {
                    return;
                }
                if (Ret != ERROR_SUCCESS)
                {
                    throw CreateErrorWin32(Ret);
                }

                IoTransact& Call = OvlEx->Call;

                if (Call.Request.Hint == 0)
                {
                    Call.IsResponseValid = false;
                    continue;
                }

                Call.IsResponseValid = true;
                switch (Call.Request.Kind)
                {
                case TransactKind::Read:
                    Read(Call.DataBuffer, Call.Request.Op.Read.BlockAddress, Call.Request.Op.Read.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Write:
                    Write(Call.DataBuffer, Call.Request.Op.Write.BlockAddress, Call.Request.Op.Write.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Flush:
                    Flush(Call.Request.Op.Flush.BlockAddress, Call.Request.Op.Flush.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Unmap:
                    std::for_each_n(reinterpret_cast<const UnmapDescriptor*>(Call.DataBuffer), Call.Request.Op.Unmap.Count, [this](const UnmapDescriptor& Descriptor) {
                        Unmap(Descriptor.BlockAddress, Descriptor.BlockCount);
                    });
                    Call.Response.Status = {};
                    break;
                default:
                    Call.Response.Status = {
                        .ScsiStatus = SCSISTAT_CHECK_CONDITION,
                        .SenseKey = SCSI_SENSE_ILLEGAL_REQUEST,
                        .ASC = SCSI_ADSENSE_ILLEGAL_COMMAND
                    };
                    break;
                }
            }
        }
        catch (const std::system_error& e)
        {
            printf("%x %s\n", e.code().value(), e.what());
        }
        catch (const std::exception& e)
        {
            printf("%s\n", e.what());
        }
    }
    // NOLINTEND(modernize-use-nullptr)
}