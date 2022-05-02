#pragma once

#include "Handle.h"

#include <guiddef.h>

#include <array>
#include <thread>
#include <vector>

namespace FastSpd
{
    struct OverlappedExPublic
    {
        std::array<std::byte, 32> Overlapped;
        std::array<std::byte, 72> Call;
    };

    class VirtualDisk
    {
        static constexpr size_t ThreadCount = 8;
        static constexpr size_t CallCount = ThreadCount * 2;
        static constexpr size_t MaxTransferLength = 1 << 21; // 2 MB

    public:
        VirtualDisk(uint64_t BlockCount, uint32_t BlockSize);

        virtual ~VirtualDisk();

        void Start();

        void Stop();

        void List();

        virtual void Read(std::byte* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Write(const std::byte* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

    private:
        void ThreadFunc(size_t Idx);

        void* DeviceHandle;
        uint32_t Btl;
        GUID Guid;
        void* IocpHandle;
        std::unique_ptr<std::byte[]> DataBuffer;
        std::array<OverlappedExPublic, CallCount> IocpRange;
        std::array<std::thread, CallCount> Threads;
    };
}