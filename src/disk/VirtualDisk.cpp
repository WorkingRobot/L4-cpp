#include "VirtualDisk.h"

#include "../utils/Error.h"
#include "../utils/Random.h"

#include <winspd/winspd.h>

namespace L4
{
    constexpr char ProductId[sizeof(SPD_STORAGE_UNIT_PARAMS::ProductId) + 1] = "L4";
    constexpr char ProductRevisionLevel[sizeof(SPD_STORAGE_UNIT_PARAMS::ProductRevisionLevel) + 1] = "3.9";

    static BOOLEAN ReadCallback(SPD_STORAGE_UNIT* StorageUnit,
        PVOID Buffer, UINT64 BlockAddress, UINT32 BlockCount, BOOLEAN Flush,
        SPD_STORAGE_UNIT_STATUS* Status) noexcept
    {
        ((VirtualDisk*)StorageUnit->UserContext)->Read(Buffer, BlockAddress, BlockCount);
        return TRUE;
    }

    static BOOLEAN WriteCallback(SPD_STORAGE_UNIT* StorageUnit,
        PVOID Buffer, UINT64 BlockAddress, UINT32 BlockCount, BOOLEAN Flush,
        SPD_STORAGE_UNIT_STATUS* Status) noexcept
    {
        ((VirtualDisk*)StorageUnit->UserContext)->Write(Buffer, BlockAddress, BlockCount);
        return TRUE;
    }

    static BOOLEAN FlushCallback(SPD_STORAGE_UNIT* StorageUnit,
        UINT64 BlockAddress, UINT32 BlockCount,
        SPD_STORAGE_UNIT_STATUS* Status) noexcept
    {
        ((VirtualDisk*)StorageUnit->UserContext)->Flush(BlockAddress, BlockCount);
        return TRUE;
    }

    static BOOLEAN UnmapCallback(SPD_STORAGE_UNIT* StorageUnit,
        SPD_UNMAP_DESCRIPTOR Descriptors[], UINT32 Count,
        SPD_STORAGE_UNIT_STATUS* Status) noexcept
    {
        while (Count)
        {
            ((VirtualDisk*)StorageUnit->UserContext)->Unmap(Descriptors->BlockAddress, Descriptors->BlockCount);
            Count--;
            Descriptors++;
        }
        return TRUE;
    }

    static constexpr SPD_STORAGE_UNIT_INTERFACE Interface{
        .Read = ReadCallback,
        .Write = WriteCallback,
        .Flush = FlushCallback,
        .Unmap = UnmapCallback
    };

    VirtualDisk::VirtualDisk(uint64_t BlockCount, uint32_t BlockSize)
    {
        SpdDebugLogSetHandle(GetStdHandle(STD_ERROR_HANDLE));

        SPD_STORAGE_UNIT_PARAMS Params{
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
        RandomGuid((char*)&Params.Guid);

        auto Status = SpdStorageUnitCreate(NULL, &Params, &Interface, (SPD_STORAGE_UNIT**)&Unit);
        if (Status != STATUS_SUCCESS)
        {
            throw CreateErrorWin32(Status, __FUNCTION__);
        }
        SpdStorageUnitSetDebugLog(((SPD_STORAGE_UNIT*)Unit), 0);

        ((SPD_STORAGE_UNIT*)Unit)->UserContext = this;
    }

    VirtualDisk::~VirtualDisk()
    {
        SpdStorageUnitDelete((SPD_STORAGE_UNIT*)Unit);
    }

    void VirtualDisk::Start()
    {
        auto Status = SpdStorageUnitStartDispatcher((SPD_STORAGE_UNIT*)Unit, 0);
        if (Status != STATUS_SUCCESS)
        {
            throw CreateErrorWin32(Status, __FUNCTION__);
        }
    }

    void VirtualDisk::Stop()
    {
        SpdStorageUnitShutdown((SPD_STORAGE_UNIT*)Unit);
    }
}