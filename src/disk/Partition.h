#pragma once

#include <stdint.h>

namespace L4::Disk
{
    struct Partition
    {
        uint64_t BlockAddress;
        uint64_t BlockCount;
        uint8_t Type;
    };
}