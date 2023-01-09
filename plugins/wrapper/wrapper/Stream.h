#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <span>

namespace L4::Plugin::Wrapper
{
    class Stream
    {
    public:
        Stream(libL4::Handle ClientHandle);

        libL4::Marshal::StreamIdentity GetIdentity() const;

        void SetIdentity(const libL4::Marshal::StreamIdentity& NewIdentity);

        void ReadContext(std::span<std::byte> Dst) const;

        void WriteContext(std::span<const std::byte> Src);

        uint64_t GetCapacity() const;

        uint64_t GetSize() const;

        void ReserveCapacity(uint64_t NewCapacity);

        void ShrinkToFit();

        void Resize(uint64_t NewSize);

        void ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const;

        void WriteBytes(std::span<const std::byte> Src, uint64_t Offset);

    private:
        libL4::Handle ClientHandle;
    };
}