#pragma once

#include "../Entry.h"

namespace L4
{
    struct ContinuatorClusterEntry : public EntryNonResidentCluster
    {
        
    };

    template<>
    struct EntryTraits<ContinuatorClusterEntry> : public EntryTraitsBase<ContinuatorClusterEntry>
    {
        static constexpr uint8_t Id = 0xFE;
    };
}