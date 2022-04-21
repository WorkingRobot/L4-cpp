#pragma once

//#include "disk/VirtualDisk.h"
#include <fastspd.h>

#include <array>
#include <unordered_map>

namespace L4
{
    class RamDisk : public FastSpd::VirtualDisk
    {
    public:
        static constexpr uint64_t BlockSize = 1 << 12;
        static constexpr uint64_t DiskSize = 1ll << 21;

        static constexpr uint64_t BlockCount = DiskSize / BlockSize;
        static_assert(DiskSize % BlockSize == 0, "Disk size must be a multiple of block size");

        RamDisk();

        void Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

        void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept override;

    private:
        //std::unordered_map<uint64_t, std::array<char, BlockSize>> Disk;
    };
}