#pragma once

#include "../Entry.h"

namespace L4
{
    struct ContinuatorSectorEntry : public EntryNonResidentSector
    {
        
    };

    template<>
    struct EntryTraits<ContinuatorSectorEntry> : public EntryTraitsBase<ContinuatorSectorEntry>
    {
        static constexpr uint8_t Id = 0xFD;
    };
}