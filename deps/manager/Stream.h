#pragma once

#include "archive/Stream.h"
#include "archive/StreamWritable.h"

#include <libL4/marshal/Marshal.h>

namespace L4::Manager
{
    class Stream
    {
    public:
        Stream(L4::Archive::Stream&& Stream);

        Stream(L4::Archive::StreamWritable&& Stream);

        libL4::Marshal::StreamIdentity GetIdentity() const;

        void SetIdentity(const libL4::Marshal::StreamIdentity& NewIdentity);

        void ReadContext(std::span<std::byte, sizeof(L4::Archive::StreamHeader::Context)> Dst) const;

        void WriteContext(std::span<const std::byte, sizeof(L4::Archive::StreamHeader::Context)> Src);

        uint64_t GetCapacity() const;

        uint64_t GetSize() const;

        void ReserveCapacity(uint64_t NewCapacity);

        void ShrinkToFit();

        void Resize(uint64_t NewSize);

        void ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const;

        void WriteBytes(std::span<const std::byte> Src, uint64_t Offset);

    private:
        std::variant<L4::Archive::Stream, L4::Archive::StreamWritable> Impl;
    };
}