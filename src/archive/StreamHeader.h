#pragma once

#include <type_traits>

namespace L4
{
    struct StreamHeader
    {
        char8_t Id[32];
        char8_t Name[32];
        uint32_t Version;
        uint32_t ElementSize;
        std::byte Context[184];
    };

    static_assert(sizeof(StreamHeader) == 256);
    static_assert(std::has_unique_object_representations_v<StreamHeader>);
}