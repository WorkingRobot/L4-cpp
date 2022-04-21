#include "BasicDisk.h"

namespace L4
{
    BasicDisk::BasicDisk() :
        VirtualDisk(BlockCount, BlockSize),
        Filesystem(Partition.BlockAddress, Partition.BlockCount)
    {
        Data = CreateGPT(&Partition, 1, BlockSize, BlockCount);

        IntervalList Ints;
        Ints.Add(0, 1, Data.ProtectiveMBR.Data);
        Ints.Add(1, 1, Data.ProtectiveMBR.Data);

        auto PrimaryTableStartOffset = 2;
        auto PrimaryTableSize = TableBlockSize;
        Ints.Add(PrimaryTableStartOffset, PrimaryTableSize, Data.Table.Data);

        auto SecondaryTableStartOffset = BasicDisk::BlockCount - 1 - TableBlockSize;
        auto SecondaryTableSize = TableBlockSize;
        Ints.Add(SecondaryTableStartOffset, SecondaryTableSize, Data.Table.Data);

        Ints.Add(BasicDisk::BlockCount - 1, 1, Data.SecondaryHeader.Data);

        Ints.Merge(256, Filesystem.GetIntervalList());

        Tree = IntervalTree(std::move(Ints));
    }

    void BasicDisk::Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("READ %llu %u\n", BlockAddress, BlockCount);
        memset(Buffer, 0, (uint64_t)BlockCount * BlockSize);

        Tree.Get(BlockAddress, BlockCount, [Buffer, BlockAddress, BlockCount](const auto& Interval)
        {
            printf("%llu - %llu\n", Interval.Start, Interval.End);

            auto Offset = BlockAddress - Interval.Start; // Offset within the interval
            auto Count = std::min(BlockAddress + BlockCount, Interval.End + 1) - BlockAddress; // Blocks to handle for this interval
            auto ByteCount = std::min(Count * BlockSize, Interval.BufferSize - Offset * BlockSize); // Bytes available to copy to the buffer
            auto BufferOffset = std::max(BlockAddress, Interval.Start) - BlockAddress; // In blocks
            memcpy((uint8_t*)Buffer + BufferOffset * BlockSize, (uint8_t*)Interval.Buffer + Offset * BlockSize, ByteCount);
        });
    }

    void BasicDisk::Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("WRITE %llu %u\n", BlockAddress, BlockCount);
        while (BlockCount)
        {
            auto Itr = RamDisk.try_emplace(BlockAddress);
            memcpy(Itr.first->second.data(), Buffer, BlockSize);
            ++BlockAddress;
            BlockCount--;
            Buffer = (char*)Buffer + BlockSize;
        }
    }

    void BasicDisk::Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("FLUSH %llu %u\n", BlockAddress, BlockCount);

    }

    void BasicDisk::Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("UNMAP %llu %u\n", BlockAddress, BlockCount);

        if (BlockCount > 0x1000000)
        {
            printf("Skipping unmap\n");
            return;
        }

        while (BlockCount)
        {
            RamDisk.erase(BlockAddress);
            ++BlockAddress;
            BlockCount--;
        }
    }
}