#include "BasicDisk.h"

namespace L4::Disk
{
    BasicDisk::BasicDisk(const ExFatDirectory& ExFatTree) :
        VirtualDisk(BlockCount, BlockSize),
        GPTData(GPT::Create(BlockSize, BlockCount, &Partition, 1)),
        Filesystem(Partition.BlockAddress, Partition.BlockCount, ExFatTree)
    {
        IntervalList Ints;
        Ints.Add(0, 1, GPTData.ProtectiveMBR);
        Ints.Add(1, 1, GPTData.ProtectiveMBR);

        auto PrimaryTableStartOffset = 2;
        auto PrimaryTableSize = TableBlockSize;
        Ints.Add(PrimaryTableStartOffset, PrimaryTableSize, GPTData.Table);

        auto SecondaryTableStartOffset = BasicDisk::BlockCount - 1 - TableBlockSize;
        auto SecondaryTableSize = TableBlockSize;
        Ints.Add(SecondaryTableStartOffset, SecondaryTableSize, GPTData.Table);

        Ints.Add(BasicDisk::BlockCount - 1, 1, GPTData.SecondaryHeader);

        Ints.Merge(256, Filesystem.GetIntervalList());

        Tree = IntervalTree(std::move(Ints));
    }

    void BasicDisk::Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        memset(Buffer, 0, (uint64_t)BlockCount * BlockSize);
        Tree.Get(BlockAddress, BlockCount, [&](Interval Int)
        {
            if (Int.End < BlockAddress)
            {
                printf("Bad\n");
                return;
            }
            if (Int.Start >= BlockAddress + BlockCount)
            {
                printf("Bad 2\n");
                return;
            }

            if (Int.Start < BlockAddress)
            {
                auto Off = BlockAddress - Int.Start;
                if (Off * BlockSize >= Int.Buffer.size())
                {
                    return;
                }

                Int.Start = BlockAddress;
                Int.Buffer = Int.Buffer.subspan(Off * BlockSize, Int.Buffer.size() - Off * BlockSize);
            }
            if (Int.End > BlockAddress + BlockCount - 1)
            {
                Int.End = BlockAddress + BlockCount - 1;
                if (Int.Buffer.size() > (Int.End + 1 - Int.Start) * BlockSize)
                {
                    Int.Buffer = Int.Buffer.subspan(0, (Int.End + 1 - Int.Start) * BlockSize);
                }
            }

            auto Offset = Int.Start - BlockAddress;
            auto Count = Int.End - Int.Start + 1;
            auto ByteCount = std::min(Int.Buffer.size(), Count * BlockSize);
            memcpy((std::byte*)Buffer + Offset * BlockSize, Int.Buffer.data(), ByteCount);
        });

        while (BlockCount)
        {
            auto Itr = RamDisk.find(BlockAddress);
            if (Itr != RamDisk.end())
            {
                memcpy(Buffer, Itr->second.data(), BlockSize);
            }
            ++BlockAddress;
            BlockCount--;
            Buffer = (std::byte*)Buffer + BlockSize;
        }
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
            Buffer = (std::byte*)Buffer + BlockSize;
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