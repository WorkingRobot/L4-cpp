#pragma once

#include <array>

namespace L4::Disk::MBR
{
    struct Partition
    {
        uint32_t BlockAddress;
        uint32_t BlockCount;
        uint8_t Type;
    };

    using MBR = std::array<std::byte, 512>;

    MBR Create(const Partition* Partitions, uint8_t PartitionCount);
}