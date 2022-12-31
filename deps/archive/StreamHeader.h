#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct StreamHeader
    {
        char8_t Id[32];
        uint32_t Version;
        uint32_t ElementSize;
        std::byte Reserved[88];
        std::byte Context[128];
    };

    static_assert(sizeof(StreamHeader) == 256);
}