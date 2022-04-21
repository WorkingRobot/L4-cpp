#pragma once

#include <atomic>
#include <array>
#include <future>
#include <vector>

#include <guiddef.h>

#include "IoCtl.h"

namespace FastSpd
{
    struct OverlappedEx
    {
        std::array<char, 32> Base;
        IoTransact Call;
    };

    class VirtualDisk
    {
        static constexpr size_t ThreadCount = 16;
        static constexpr size_t CallCount = ThreadCount * 2;

    public:
        VirtualDisk(uint64_t BlockCount, uint32_t BlockSize);

        virtual ~VirtualDisk();

        void Start();

        void Stop();

        void List();

        virtual void Read(void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Write(const void* Buffer, uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Flush(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

        virtual void Unmap(uint64_t BlockAddress, uint32_t BlockCount) noexcept = 0;

    private:
        void ThreadFunc(size_t Idx);

        void* DeviceHandle;
        uint32_t Btl;
        GUID Guid;
        uint32_t MaxTransferLength;
        void* IocpHandle;
        std::unique_ptr<char[]> DataBuffer;
        std::array<OverlappedEx, CallCount> IocpRange;
        std::array<std::thread, CallCount> Threads;
    };
}