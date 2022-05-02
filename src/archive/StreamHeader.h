#pragma once

#include <type_traits>

namespace L4
{
    struct StreamHeader
    {
        Guid Guid;
        uint32_t Version;
        uint32_t ElementSize;
        char8_t Name[40];
        std::byte Context[192];
    };

    static_assert(sizeof(StreamHeader) == 256);
    static_assert(std::has_unique_object_representations_v<StreamHeader>);
}