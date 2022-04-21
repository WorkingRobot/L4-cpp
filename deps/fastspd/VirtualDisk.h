#pragma once

#include <atomic>
#include <future>
#include <vector>

#include <guiddef.h>

namespace FastSpd
{
    class VirtualDisk
    {
    public:
        VirtualDisk(uint64_t BlockCount, uint32_t BlockSize);

        virtual ~VirtualDisk();

        void Start();

        void Stop();

        virtual void Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        void ThreadFunc();
    private:
        void ThreadFunc2();

        void* DeviceHandle;
        uint32_t Btl;
        GUID Guid;
        uint32_t MaxTransferLength;
        std::vector<std::future<void>> Threads;
        std::atomic_flag ThreadFlag;
    };
}