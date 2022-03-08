#pragma once

#include "../Entry.h"

namespace L4
{
    struct SectorTableEntry : public EntryNonResidentCluster
    {

    };

    template<>
    struct EntryTraits<SectorTableEntry> : public EntryTraitsBase<SectorTableEntry>
    {
        static constexpr uint8_t Id = 0x03;
    };
}