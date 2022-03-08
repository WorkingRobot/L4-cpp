#pragma once

#include "../Entry.h"
#include "../Guid.h"

namespace L4
{
    struct SDTEntry : public EntryNonResidentCluster
    {
        Guid Guid;
    };

    template<>
    struct EntryTraits<SDTEntry> : public EntryTraitsBase<SDTEntry>
    {
        static constexpr uint8_t Id = 0x04;
    };
}