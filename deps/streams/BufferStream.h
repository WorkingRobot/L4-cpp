#pragma once

#include "Stream.h"

namespace L4
{
    class BufferStream : public Stream
    {
    public:
        BufferStream(std::span<std::byte> Data) noexcept;

        void WriteBytes(std::span<const std::byte> Src) override;

        void ReadBytes(std::span<std::byte> Dst) override;

        void Seek(ptrdiff_t Position, SeekPosition SeekFrom) override;

        size_t Tell() const override;

        size_t Size() const override;

        std::span<const std::byte> GetBuffer() const noexcept;

    private:
        std::span<std::byte> Data;
        size_t Position;
    };
}