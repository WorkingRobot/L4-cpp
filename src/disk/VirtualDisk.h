#pragma once

#include <stdint.h>

namespace L4
{
    class VirtualDisk
    {
    public:
        VirtualDisk(uint64_t BlockCount, uint32_t BlockSize);

        ~VirtualDisk();

        void Start();

        void Stop();

        virtual void Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

    private:
        using VD_SPD_STORAGE_UNIT = void;
        VD_SPD_STORAGE_UNIT* Unit;
    };
}