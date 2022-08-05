#pragma once

#include <cstdint>

namespace L4::Interface
{
    struct Guid
    {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;

        auto operator<=>(const Guid&) const = default;
    };
}