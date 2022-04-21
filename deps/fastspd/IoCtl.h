#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace FastSpd
{
    struct StorageUnitParams
    {
        GUID Guid;
        UINT64 BlockCount;
        UINT32 BlockLength;
        UCHAR ProductId[16];
        UCHAR ProductRevisionLevel[4];
        UINT8 DeviceType;
        UINT32 WriteProtected : 1;
        UINT32 CacheSupported : 1;
        UINT32 UnmapSupported : 1;
        UINT32 EjectDisabled : 1;
        UINT32 MaxTransferLength;
        UINT64 Reserved[8];
    };

    struct StorageUnitStatus
    {
        UINT8 ScsiStatus;
        UINT8 SenseKey;
        UINT8 ASC;
        UINT8 ASCQ;
        UINT64 Information;
        UINT64 ReservedCSI;
        UINT32 ReservedSKS;
        UINT32 ReservedFRU : 8;
        UINT32 InformationValid : 1;
    };

    struct UnmapDescriptor
    {
        UINT64 BlockAddress;
        UINT32 BlockCount;
        UINT32 Reserved;
    };

    enum class TransactKind : UINT8
    {
        Reserved = 0,
        Read,
        Write,
        Flush,
        Unmap,
        Count,
    };

    template<char Id>
    struct IoCall;

    struct alignas(8) IoCallBase
    {
        UINT16 Size;
        UINT16 Code;
    };

    template<>
    struct IoCall<'p'> : public IoCallBase
    {
        union
        {
            struct
            {
                StorageUnitParams StorageUnitParams;
            } Params;
            struct
            {
                UINT32 Btl;
            } Return;
        };
    };

    template<>
    struct IoCall<'u'> : public IoCallBase
    {
        union
        {
            struct
            {
                GUID Guid;
            } Params;
            struct
            {

            } Return;
        };
    };

    template<>
    struct IoCall<'l'> : public IoCallBase
    {
        union
        {
            struct
            {

            } Params;
            struct
            {

            } Return;
        };
    };

    template<>
    struct IoCall<'t'> : public IoCallBase
    {
        UINT32 Btl;
        UINT32 IsRequestValid : 1;
        UINT32 IsResponseValid : 1;
        UINT64 DataBuffer;
        union
        {
            struct
            {
                UINT64 Hint;
                TransactKind Kind;
                union
                {
                    struct
                    {
                        UINT64 BlockAddress;
                        UINT32 BlockCount;
                        UINT32 ForceUnitAccess : 1;
                        UINT32 Reserved : 31;
                    } Read;
                    struct
                    {
                        UINT64 BlockAddress;
                        UINT32 BlockCount;
                        UINT32 ForceUnitAccess : 1;
                        UINT32 Reserved : 31;
                    } Write;
                    struct
                    {
                        UINT64 BlockAddress;
                        UINT32 BlockCount;
                    } Flush;
                    struct
                    {
                        UINT32 Count;
                    } Unmap;
                } Op;
            } Request;
            struct
            {
                UINT64 Hint;
                UINT8 Kind;
                StorageUnitStatus Status;
            } Response;
        };
    };

    template<>
    struct IoCall<'i'> : public IoCallBase
    {
        union
        {
            struct
            {
                UINT32 Btl;
                UINT32 ProcessId;
            } Params;
            struct
            {
                UINT32 Btl;
                UINT32 ProcessId;
            } Return;
        };
    };

    using IoProvisionParams = IoCall<'p'>;
    using IoUnprovisionParams = IoCall<'u'>;
    using IoList = IoCall<'l'>;
    using IoTransact = IoCall<'t'>;
    using IoSetTransactPid = IoCall<'i'>;

    template<char Id>
    static constexpr IoCall<Id> CreateIoCall()
    {
        IoCall<Id> Call{};
        Call.Size = sizeof(IoCall<Id>);
        Call.Code = Id;
        return Call;
    }

    template<char Id, class... ArgTs>
    static constexpr IoCall<Id> CreateIoParams(const decltype(IoCall<Id>::Params)& Params)
    {
        IoCall<Id> Call = CreateIoCall<Id>();
        Call.Params = Params;
        return Call;
    }
}