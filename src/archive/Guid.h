#pragma once

#include <stdint.h>

namespace L4 {
    struct Guid {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
    };
}