#pragma once

#include "../Entry.h"

namespace L4
{
    struct NameEntry : public EntryResident
    {
        char Runlist[28];
    };

    template<>
    struct EntryTraits<NameEntry> : public EntryTraitsBase<NameEntry>
    {
        static constexpr uint8_t Id = 0x08;
    };
}