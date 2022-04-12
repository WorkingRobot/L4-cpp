#pragma once

#include <type_traits>

namespace L4
{
    struct StreamRun
    {
        uint32_t StreamSectorOffset;
        uint32_t SectorOffset;
        uint32_t SectorCount;
        uint32_t Reserved;
    };

    static_assert(sizeof(StreamRun) == 16);
    static_assert(std::has_unique_object_representations_v<StreamRun>);
}