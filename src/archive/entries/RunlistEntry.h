#pragma once

#include "../Entry.h"

namespace L4
{
    struct RunlistEntry : public EntryResident
    {
        char Runlist[28];
    };

    template<>
    struct EntryTraits<RunlistEntry> : public EntryTraitsBase<RunlistEntry>
    {
        static constexpr uint8_t Id = 0x09;
    };
}