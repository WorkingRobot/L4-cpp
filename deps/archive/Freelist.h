#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct Freelist
    {
        struct Entry
        {
            uint32_t SectorCount;
            uint32_t SectorOffset;
        };

        uint32_t EntryCount;
        uint32_t TotalSectorCount;
        Entry Entries[4095];
    };

    static_assert(sizeof(Freelist::Entry) == 8);
    static_assert(sizeof(Freelist) == 32768);
}