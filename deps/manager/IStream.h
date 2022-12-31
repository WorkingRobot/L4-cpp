#pragma once

#include <libL4/marshal/Marshal.h>

namespace L4::Manager
{
    class IStream
    {
    public:
        IStream() = default;

        virtual ~IStream() = default;

        virtual uint32_t GetIdx() const = 0;

        virtual libL4::Marshal::StreamIdentity GetIdentity() const = 0;

        virtual void SetIdentity(const libL4::Marshal::StreamIdentity& NewIdentity) = 0;

        virtual uint32_t GetElementSize() const = 0;

        virtual void SetElementSize(uint32_t NewElementSize) = 0;

        virtual void ReadContext(std::span<std::byte> Dst) const = 0;

        virtual void WriteContext(std::span<const std::byte> Src) = 0;

        virtual uint64_t GetCapacity() const = 0;

        virtual uint64_t GetSize() const = 0;

        virtual void ReserveCapacity(uint64_t NewCapacity) = 0;

        virtual void ShrinkToFit() = 0;

        virtual void Resize(uint64_t NewSize) = 0;

        virtual uint64_t ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const = 0;

        virtual uint64_t WriteBytes(std::span<const std::byte> Src, uint64_t Offset) = 0;
    };
}