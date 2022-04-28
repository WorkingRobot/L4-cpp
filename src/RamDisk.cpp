#include "RamDisk.h"

namespace L4 {
    RamDisk::RamDisk() :
        VirtualDisk(BlockCount, BlockSize)
    {
    }

    void RamDisk::Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        // printf("READ %llu %u\n", BlockAddress, BlockCount);

        while (BlockCount) {
            auto Itr = Disk.find(BlockAddress);
            if (Itr != Disk.end()) {
                memcpy(Buffer, Itr->second.data(), BlockSize);
            } else {
                memset(Buffer, 0, BlockSize);
            }
            ++BlockAddress;
            BlockCount--;
            Buffer = (char*)Buffer + BlockSize;
        }
    }

    void RamDisk::Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        // printf("WRITE %llu %u\n", BlockAddress, BlockCount);

        while (BlockCount) {
            auto Itr = Disk.try_emplace(BlockAddress);
            memcpy(Itr.first->second.data(), Buffer, BlockSize);
            ++BlockAddress;
            BlockCount--;
            Buffer = (char*)Buffer + BlockSize;
        }
    }

    void RamDisk::Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("FLUSH %llu %u\n", BlockAddress, BlockCount);
    }

    void RamDisk::Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept
    {
        printf("UNMAP %llu %u\n", BlockAddress, BlockCount);

        if (BlockCount > 0x1000000) {
            printf("Skipping unmap\n");
            return;
        }

        while (BlockCount) {
            Disk.erase(BlockAddress);
            ++BlockAddress;
            BlockCount--;
        }
    }
}