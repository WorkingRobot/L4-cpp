#pragma once

#include "MBR.h"

namespace L4
{
    struct GPTPartition
    {
        uint64_t BlockAddress;
        uint64_t BlockCount;
        uint8_t Type;
    };

    struct GPTHeader
    {
        char Data[92];
    };

    struct GPTTable
    {
        char Data[128 * 128];
    };

    struct GPTData
    {
        MBR ProtectiveMBR;
        GPTHeader PrimaryHeader;
        GPTHeader SecondaryHeader;
        GPTTable Table;
    };

    GPTData CreateGPT(const GPTPartition* Partitions, size_t PartitionCount, size_t BlockSize, size_t DiskBlockCount);
}