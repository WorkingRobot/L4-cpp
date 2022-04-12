#include "disk/VirtualDisk.h"

#include <string.h>
#include <conio.h>
#define NOMINMAX
#include <winspd/winspd.h>

#include "disk/GPT.h"
#include "disk/exFAT.h"
#include "utils/Align.h"
#include "utils/Random.h"
#include "tree/IntervalTree.h"

#include <array>
#include <unordered_map>
#include <memory>

#include "archive/StreamView.h"

namespace L4
{
    class BasicDisk : public VirtualDisk
    {
    public:
        static constexpr uint64_t BlockSize = 1<<12;
        static constexpr uint64_t DiskSize = 1ll<<40;

        static constexpr uint64_t BlockCount = DiskSize / BlockSize;
        static_assert(DiskSize % BlockSize == 0, "Disk size must be a multiple of block size");

        static constexpr uint64_t TableBlockSize = Align<BlockSize>(sizeof(GPTTable)) / BlockSize;

        BasicDisk() : VirtualDisk(BlockCount, BlockSize)
        {
            GPTPartition Partition{
                .BlockAddress = 256,//2 + TableBlockSize,
                .BlockCount = 268434944 //BlockCount - 1 - TableBlockSize - Partition.BlockAddress
            };
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

        void Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override
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

        void Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override
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

        void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept override
        {
            printf("FLUSH %llu %u\n", BlockAddress, BlockCount);

        }

        void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept override
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

    private:
        std::unordered_map<uint64_t, std::array<char, BlockSize>> RamDisk;
        GPTData Data;
        ExFatSystemPublic Filesystem;
        IntervalTree Tree;
    };

    void Main()
    {
        //MmioFileWritable File("mmfile");
        //CreateArchive(File);
        /*BasicDisk Disk;
        Disk.Start();
        _getch();
        Disk.Stop();*/
    }
}

int main(int argc, char* argv[])
{
    L4::Main();

    return true;
}