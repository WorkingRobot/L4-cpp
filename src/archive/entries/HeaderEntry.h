#pragma once

#include "ContinuatorClusterEntry.h"
#include "../Guid.h"

namespace L4
{
    struct HeaderEntry : public ContinuatorClusterEntry
    {
        uint16_t Flags;
        uint16_t StorageFlags;
        uint32_t Reserved;
        Guid SourceGuid;
        Guid AppGuid;
    };

    template<>
    struct EntryTraits<HeaderEntry> : public EntryTraitsBase<HeaderEntry>
    {
        static constexpr uint8_t Id = 0xFF;
    };
}