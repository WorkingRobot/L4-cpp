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
#define _NTSCSI_USER_MODE_
#include <scsi.h>
// clang-format on

#include <ranges>
#include <string>

namespace FastSpd {
    static std::string GetDevicePath()
    {
        static constexpr std::string_view DeviceName = "root\\winspd";
        static constexpr std::string_view GLOBALROOT = "\\\\?\\GLOBALROOT";

        using DiHandle = Handle<SetupDiDestroyDeviceInfoList, HDEVINFO, ~0llu>;

        DiHandle Handle = SetupDiGetClassDevsA(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

        if (!Handle) {
            throw CreateErrorWin32(GetLastError());
        }

        SP_DEVINFO_DATA Info { .cbSize = sizeof(Info) };
        for (DWORD Idx = 0; SetupDiEnumDeviceInfo(Handle, Idx, &Info); ++Idx) {
            BYTE HwidBuf[256] {};
            if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_HARDWAREID, NULL, HwidBuf, sizeof(HwidBuf) - 2, NULL)) {
                continue;
            }

            PCSTR HwidBufPtr = (PCHAR)HwidBuf;
            for (std::string_view Id(HwidBufPtr); !Id.empty(); HwidBufPtr += Id.size() + 1, Id = HwidBufPtr) {
                static constexpr auto to_lower = std::ranges::views::transform([](char c) { return std::tolower(c); });
                if (std::ranges::equal(Id | to_lower, DeviceName)) {
                    DWORD NameSize;
                    SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, NULL, 0, &NameSize);
                    std::string Ret(GLOBALROOT.size() + NameSize, '\0');
                    GLOBALROOT.copy(Ret.data(), GLOBALROOT.size());
                    if (!SetupDiGetDeviceRegistryPropertyA(Handle, &Info, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (PBYTE)Ret.data() + GLOBALROOT.size(), NameSize, NULL)) {
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

        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, NULL)) {
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
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, &Overlapped)) {
            DWORD Error = GetLastError();
            if (Error == ERROR_IO_PENDING) {
                DWORD BytesTransferred;
                if (!GetOverlappedResult(DeviceHandle, &Overlapped, &BytesTransferred, TRUE)) {
                    throw CreateErrorWin32(GetLastError());
                }
            } else {
                throw CreateErrorWin32(GetLastError());
            }
        }
    }

    static void List(HANDLE DeviceHandle)
    {
        union {
            IoList Call = CreateIoCall<'l'>();
            UINT32 Bitmap[256];
        };

        DWORD BytesTransferred;
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Bitmap, sizeof(Bitmap), &BytesTransferred, NULL)) {
            throw CreateErrorWin32(GetLastError());
        }

        for (ULONG Idx = 0; Idx < BytesTransferred / 4; ++Idx) {
            UINT32 Btl = Bitmap[Idx];
            printf("%d %d %d\n", (((Btl) >> 16) & 0xff), (((Btl) >> 8) & 0xff), ((Btl)&0xff));
        }
    }

    static void Transact(HANDLE DeviceHandle, IoTransact& Call, LPOVERLAPPED Overlapped)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call, sizeof(Call), &Call, sizeof(Call), NULL, Overlapped)) {
            DWORD Error = GetLastError();
            if (Error == ERROR_IO_PENDING) {
                DWORD BytesTransferred;
                if (!GetOverlappedResult(DeviceHandle, Overlapped, &BytesTransferred, TRUE)) {
                    throw CreateErrorWin32(GetLastError());
                }
            } else {
                throw CreateErrorWin32(GetLastError());
            }
        }
    }

    static void DispatchTransact(HANDLE DeviceHandle, OverlappedEx& Call)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &Call.Call, sizeof(Call.Call), &Call.Call, sizeof(Call.Call), NULL, (LPOVERLAPPED)Call.Base.data())) {
            DWORD Error = GetLastError();
            if (Error != ERROR_IO_PENDING) {
                throw CreateErrorWin32(GetLastError());
            }
        }
    }

    static DWORD Transact(HANDLE DeviceHandle, HANDLE IocpHandle, OverlappedEx** OverlappedEx)
    {
        if (!DeviceIoControl(DeviceHandle, IOCTL_MINIPORT_PROCESS_SERVICE_IRP, &(*OverlappedEx)->Call, sizeof(IoTransact), &(*OverlappedEx)->Call, sizeof(IoTransact), NULL, (LPOVERLAPPED)(*OverlappedEx)->Base.data())) {
            DWORD Error = GetLastError();
            if (Error != ERROR_IO_PENDING) {
                throw CreateErrorWin32(GetLastError());
            }
        }

        DWORD BytesTransferred;
        ULONG_PTR CompletionKey;
        BOOL Result;
        DWORD Error;
        do {
            Result = GetQueuedCompletionStatus(IocpHandle, &BytesTransferred, &CompletionKey, (LPOVERLAPPED*)OverlappedEx, INFINITE);
            Error = GetLastError();
        } while (!Result && !*OverlappedEx && Error != ERROR_ABANDONED_WAIT_0 && Error != ERROR_INVALID_HANDLE);
        if (Result) {
            if (CompletionKey != 0) {
                return ERROR_ABANDONED_WAIT_0;
            }
            return ERROR_SUCCESS;
        } else {
            if (Error == ERROR_INVALID_HANDLE) {
                return ERROR_ABANDONED_WAIT_0;
            }
            return Error;
        }
    }

    constexpr char ProductId[sizeof(StorageUnitParams::ProductId) + 1] = "L4";
    constexpr char ProductRevisionLevel[sizeof(StorageUnitParams::ProductRevisionLevel) + 1] = "3.9";

    VirtualDisk::VirtualDisk(uint64_t BlockCount, uint32_t BlockSize)
    {
        auto Path = GetDevicePath();

        DeviceHandle = CreateFileA(Path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        if (DeviceHandle == INVALID_HANDLE_VALUE) {
            throw CreateErrorWin32(GetLastError());
        }

        StorageUnitParams Params {
            .BlockCount = BlockCount,
            .BlockLength = BlockSize,
            .DeviceType = 0,
            .WriteProtected = 1,
            .CacheSupported = 1,
            .UnmapSupported = 1,
            .EjectDisabled = 1,
            .MaxTransferLength = 1 << 21 // 2 MB
        };
        memcpy(Params.ProductId, ProductId, sizeof(Params.ProductId));
        memcpy(Params.ProductRevisionLevel, ProductRevisionLevel, sizeof(Params.ProductRevisionLevel));
        Params.Guid = UuidCreate();

        Provision(DeviceHandle, Params, Btl);

        Guid = Params.Guid;
        MaxTransferLength = Params.MaxTransferLength;
    }

    VirtualDisk::~VirtualDisk()
    {
        Stop();
        CloseHandle(DeviceHandle);
    }

    void VirtualDisk::Start()
    {
        IocpHandle = CreateIoCompletionPort(DeviceHandle, NULL, NULL, ThreadCount);
        if (IocpHandle == NULL) {
            throw CreateErrorWin32(GetLastError());
        }
        if (!SetFileCompletionNotificationModes(DeviceHandle, FILE_SKIP_SET_EVENT_ON_HANDLE)) {
            throw CreateErrorWin32(GetLastError());
        }

        DataBuffer = std::make_unique<char[]>(CallCount * MaxTransferLength);
        for (size_t Idx = 0; Idx < CallCount; ++Idx) {
            Threads[Idx] = std::thread(&VirtualDisk::ThreadFunc, this, Idx);
        }
    }

    void VirtualDisk::Stop()
    {
        OverlappedEx Temp {};
        for (auto& Thread : Threads) {
            PostQueuedCompletionStatus(IocpHandle, 0, 1, (LPOVERLAPPED)Temp.Base.data());
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
        IocpRange[Idx] = {};
        auto& Call = IocpRange[Idx].Call;
        Call = CreateIoCall<'t'>();
        Call.Btl = Btl;
        Call.DataBuffer = (UINT64)(UINT_PTR)(DataBuffer.get() + Idx * MaxTransferLength);
        Call.IsRequestValid = true;
        Call.IsResponseValid = false;

        try {
            for (OverlappedEx* OverlappedEx = &IocpRange[Idx];;) {
                DWORD Ret = Transact(DeviceHandle, IocpHandle, &OverlappedEx);
                if (Ret == ERROR_ABANDONED_WAIT_0) {
                    return;
                }
                if (Ret != ERROR_SUCCESS) {
                    throw CreateErrorWin32(Ret);
                }

                IoTransact& Call = OverlappedEx->Call;

                if (Call.Request.Hint == 0) {
                    Call.IsResponseValid = false;
                    continue;
                }

                Call.IsResponseValid = true;
                switch (Call.Request.Kind) {
                case TransactKind::Read:
                    Read(((void*)Call.DataBuffer), Call.Request.Op.Read.BlockAddress, Call.Request.Op.Read.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Write:
                    Write(((void*)Call.DataBuffer), Call.Request.Op.Write.BlockAddress, Call.Request.Op.Write.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Flush:
                    Flush(Call.Request.Op.Flush.BlockAddress, Call.Request.Op.Flush.BlockCount);
                    Call.Response.Status = {};
                    break;
                case TransactKind::Unmap:
                    std::for_each_n((UnmapDescriptor*)Call.DataBuffer, Call.Request.Op.Unmap.Count, [this](const UnmapDescriptor& Descriptor) {
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
        } catch (std::system_error e) {
            printf("%x %s\n", e.code().value(), e.what());
        } catch (std::exception e) {
            printf("%s\n", e.what());
        }
    }
}