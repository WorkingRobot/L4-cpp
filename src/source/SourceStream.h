#pragma once

#include "SourceStructs.h"

#include "../archive/StreamView.h"

namespace L4
{
    class SourceStream
    {
    public:
        SourceStream(StreamViewWritable Stream);

        [[nodiscard]] Source::StreamIdentity GetIdentity() const;

        void SetIdentity(const Source::StreamIdentity& NewIdentity);

        [[nodiscard]] uint32_t GetElementSize() const;

        void SetElementSize(uint32_t NewElementSize);

        [[nodiscard]] std::span<const std::byte, 192> GetContext() const;

        void SetContext(std::span<const std::byte, 192> NewContext);

        [[nodiscard]] uint64_t GetCapacity() const;

        [[nodiscard]] uint64_t GetSize() const;

        void ReserveCapacity(uint64_t NewCapacity);

        void ShrinkToFit();

        void Resize(uint64_t NewSize);

        uint64_t Read(std::span<std::byte> Dst, uint64_t Offset) const;

        uint64_t Write(std::span<const std::byte> Src, uint64_t Offset);

    private:
        StreamViewWritable Stream;
    };
}