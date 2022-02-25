#pragma once

#include <stdint.h>

namespace L4
{
    struct MBRPartition
    {
        uint32_t BlockAddress;
        uint32_t BlockCount;
        uint8_t Type;
    };

    struct MBR
    {
        char Data[512];
    };

    MBR CreateMBR(const MBRPartition* Partitions, size_t PartitionCount);
}