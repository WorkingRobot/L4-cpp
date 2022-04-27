#include "BasicDisk.h"

namespace L4
{
    BasicDisk::BasicDisk(const ExFatDirectory& ExFatTree) :
        VirtualDisk(BlockCount, BlockSize),
        Filesystem(Partition.BlockAddress, Partition.BlockCount, ExFatTree)
    {
        Data = CreateGPT(&Partition, 1, BlockSize, BlockCount);
        auto MBRSpan = std::span(Data.ProtectiveMBR.Data, sizeof(Data.ProtectiveMBR.Data));
        auto TableSpan = std::span(Data.Table.Data, sizeof(Data.Table.Data));
        auto SecondaryHeaderSpan = std::span(Data.SecondaryHeader.Data, sizeof(Data.SecondaryHeader.Data));

        IntervalList Ints;
        Ints.Add(0, 1, MBRSpan);
        Ints.Add(1, 1, MBRSpan);

        auto PrimaryTableStartOffset = 2;
        auto PrimaryTableSize = TableBlockSize;
        Ints.Add(PrimaryTableStartOffset, PrimaryTableSize, TableSpan);

        auto SecondaryTableStartOffset = BasicDisk::BlockCount - 1 - TableBlockSize;
        auto SecondaryTableSize = TableBlockSize;
        Ints.Add(SecondaryTableStartOffset, SecondaryTableSize, TableSpan);

        Ints.Add(BasicDisk::BlockCount - 1, 1, SecondaryHeaderSpan);

        Ints.Merge(256, Filesystem.GetIntervalList());

        Tree = IntervalTree(std::move(Ints));
    }

    void BasicDisk::Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        //printf("READ %llu %u\n", BlockAddress, BlockCount);
        memset(Buffer, 0, (uint64_t)BlockCount * BlockSize);

        /*std::ranges::for_each(Tree.Data, [&](const Interval& Int)
        {
            if (BlockAddress < Int.Start)
            {
                return;
            }
            if (BlockAddress + BlockCount - 1 > Int.End + 1)
            {
                return;
            }
            printf("[%llu - %llu]\n", Int.Start, Int.End);

            auto Offset = BlockAddress - Int.Start; // Offset within the interval
            auto Count = std::min(BlockAddress + BlockCount, Int.End + 1) - BlockAddress; // Blocks to handle for this interval
            if (Int.Buffer.size() < Offset * BlockSize)
            {
                return;
            }
            auto ByteCount = std::min(Count * BlockSize, Int.Buffer.size() - Offset * BlockSize); // Bytes available to copy to the buffer
            auto BufferOffset = std::max(BlockAddress, Int.Start) - BlockAddress; // In blocks

            std::byte* Dst = (std::byte*)Buffer + BufferOffset * BlockSize;
            std::byte* DstEnd = (std::byte*)Buffer + (uint64_t)BlockCount * BlockSize;
            std::byte* Src = Int.Buffer.data() + Offset * BlockSize;
            std::byte* SrcEnd = Int.Buffer.data() + Int.Buffer.size();
            size_t DstSize = DstEnd - Dst;
            size_t SrcSize = SrcEnd - Src;
            if (ByteCount > DstSize)
            {
                printf("Bigger than DstSize\n");
                printf("ByteCount: %zu DstSize: %zu\n", ByteCount, DstSize);
            }
            if (ByteCount > SrcSize)
            {
                printf("Bigger than SrcSize\n");
                printf("ByteCount: %zu SrcSize: %zu\n", ByteCount, SrcSize);
            }
            ByteCount = std::min<size_t>({ ByteCount, DstSize, SrcSize });
            if (IsBadReadPtr(Src, ByteCount))
            {
                __debugbreak();
            }
            if (IsBadWritePtr(Dst, ByteCount))
            {
                __debugbreak();
            }
            memcpy_s(Dst, ByteCount, Src, ByteCount);
        });*/
        Tree.Get(BlockAddress, BlockCount, [&](Interval Int)
        {
            //printf("[%llu - %llu]\n", Int.Start, Int.End);

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

            /*
            auto Offset = (ptrdiff_t)BlockAddress - (ptrdiff_t)Int.Start; // Offset within the interval
            auto Count = std::min(BlockAddress + BlockCount, Int.End + 1) - BlockAddress; // Blocks to handle for this interval
            if (Offset > 0 && Int.Buffer.size() < Offset * BlockSize)
            {
                return;
            }
            auto ByteCount = std::min(Count * BlockSize, Int.Buffer.size() - Offset * BlockSize); // Bytes available to copy to the buffer
            auto BufferOffset = std::max(BlockAddress, Int.Start) - BlockAddress; // In blocks
            memcpy((std::byte*)Buffer + BufferOffset * BlockSize, Int.Buffer.data() + Offset * BlockSize, ByteCount);*/
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
            Buffer = (char*)Buffer + BlockSize;
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