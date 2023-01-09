#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct StreamHeader
    {
        char8_t Id[32];
        std::byte Reserved[32];
        std::byte Context[192];
    };

    static_assert(sizeof(StreamHeader) == 256);
}