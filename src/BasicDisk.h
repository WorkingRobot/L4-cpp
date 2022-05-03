#pragma once

#include "disk/exFAT.h"
#include "disk/GPT.h"
#include "utils/Align.h"
#include "utils/IntervalTree.h"
#include "utils/Random.h"

#include <fastspd.h>

#include <array>
#include <unordered_map>

namespace L4::Disk
{
    class BasicDisk : public FastSpd::VirtualDisk
    {
    public:
        static constexpr uint64_t BlockSize = 1 << 12;
        static constexpr uint64_t DiskSize = 1ll << 28;

        static constexpr uint64_t BlockCount = DiskSize / BlockSize;
        static_assert(DiskSize % BlockSize == 0, "Disk size must be a multiple of block size");

        static constexpr uint64_t TableBlockSize = Align<BlockSize>(sizeof(GPT::Table)) / BlockSize;

        static constexpr Partition Partition {
            .BlockAddress = 256,
            .BlockCount = BlockCount - 256 - 256
        };

        BasicDisk(const ExFatDirectory& ExFatTree);

        void Read(std::byte* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Write(const std::byte* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

    private:
        std::unordered_map<uint64_t, std::array<std::byte, BlockSize>> RamDisk;

        GPT GPTData;
        ExFatSystem Filesystem;
        IntervalTree Tree;
    };
}