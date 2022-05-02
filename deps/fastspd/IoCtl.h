#pragma once

#include <guiddef.h>
#include <stdint.h>

#include <array>

namespace FastSpd
{
    struct StorageUnitParams
    {
        GUID Guid;
        uint64_t BlockCount;
        uint32_t BlockLength;
        std::array<char, 16> ProductId;
        std::array<char, 4> ProductRevisionLevel;
        uint8_t DeviceType;
        uint32_t WriteProtected : 1;
        uint32_t CacheSupported : 1;
        uint32_t UnmapSupported : 1;
        uint32_t EjectDisabled  : 1;
        uint32_t MaxTransferLength;
        std::array<uint64_t, 8> Reserved;
    };

    struct StorageUnitStatus
    {
        uint8_t ScsiStatus;
        uint8_t SenseKey;
        uint8_t ASC;
        uint8_t ASCQ;
        uint64_t Information;
        uint64_t ReservedCSI;
        uint32_t ReservedSKS;
        uint32_t ReservedFRU      : 8;
        uint32_t InformationValid : 1;
    };

    struct UnmapDescriptor
    {
        uint64_t BlockAddress;
        uint32_t BlockCount;
        uint32_t Reserved;
    };

    enum class TransactKind : uint8_t
    {
        Reserved = 0,
        Read,
        Write,
        Flush,
        Unmap,
        Count,
    };

    struct IoCallBase
    {
        alignas(8) uint16_t Size;
        uint16_t Code;
    };

    template <char Id>
    struct IoCall;

    template <>
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
                uint32_t Btl;
            } Return;
        };
    };

    template <>
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

    template <>
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

    template <>
    struct IoCall<'t'> : public IoCallBase
    {
        uint32_t Btl;
        uint32_t IsRequestValid  : 1;
        uint32_t IsResponseValid : 1;
        uint64_t DataBuffer;
        union
        {
            struct
            {
                uint64_t Hint;
                TransactKind Kind;
                union
                {
                    struct
                    {
                        uint64_t BlockAddress;
                        uint32_t BlockCount;
                        uint32_t ForceUnitAccess : 1;
                        uint32_t Reserved        : 31;
                    } Read;
                    struct
                    {
                        uint64_t BlockAddress;
                        uint32_t BlockCount;
                        uint32_t ForceUnitAccess : 1;
                        uint32_t Reserved        : 31;
                    } Write;
                    struct
                    {
                        uint64_t BlockAddress;
                        uint32_t BlockCount;
                    } Flush;
                    struct
                    {
                        uint32_t Count;
                    } Unmap;
                } Op;
            } Request;
            struct
            {
                uint64_t Hint;
                uint8_t Kind;
                StorageUnitStatus Status;
            } Response;
        };
    };

    template <>
    struct IoCall<'i'> : public IoCallBase
    {
        union
        {
            struct
            {
                uint32_t Btl;
                uint32_t ProcessId;
            } Params;
            struct
            {
                uint32_t Btl;
                uint32_t ProcessId;
            } Return;
        };
    };

    using IoProvisionParams = IoCall<'p'>;
    using IoUnprovisionParams = IoCall<'u'>;
    using IoList = IoCall<'l'>;
    using IoTransact = IoCall<'t'>;
    using IoSetTransactPid = IoCall<'i'>;

    template <char Id>
    static constexpr IoCall<Id> CreateIoCall()
    {
        IoCall<Id> Call {};
        Call.Size = sizeof(IoCall<Id>);
        Call.Code = Id;
        return Call;
    }

    template <char Id, class... ArgTs>
    static constexpr IoCall<Id> CreateIoParams(const decltype(IoCall<Id>::Params)& Params)
    {
        IoCall<Id> Call = CreateIoCall<Id>();
        Call.Params = Params;
        return Call;
    }
}