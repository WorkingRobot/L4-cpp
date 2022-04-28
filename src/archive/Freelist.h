#pragma once

#include <type_traits>

namespace L4 {
    struct Freelist {
        struct Entry {
            uint32_t SectorCount;
            uint32_t SectorOffset;
        };

        uint32_t EntryCount;
        uint32_t TotalSectorCount;
        Entry Entries[511];
    };

    static_assert(sizeof(Freelist) == 4096);
    static_assert(std::has_unique_object_representations_v<Freelist>);
}