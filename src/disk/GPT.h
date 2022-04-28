#pragma once

#include "MBR.h"

namespace L4::Disk::GPT
{
    struct Partition
    {
        uint64_t BlockAddress;
        uint64_t BlockCount;
        uint8_t Type;
    };

    using Header = std::array<std::byte, 92>;
    using Table = std::array<std::byte, 128 * 128>;

    struct GPT
    {
        MBR::MBR ProtectiveMBR;
        Header PrimaryHeader;
        Header SecondaryHeader;
        Table Table;
    };

    GPT Create(uint64_t BlockSize, uint64_t DiskBlockCount, const Partition* Partitions, uint8_t PartitionCount);
}