#pragma once

#include "Stream.h"

namespace L4
{
    class MemoryStream : public Stream
    {
    public:
        MemoryStream(size_t Capacity = 1024);

        MemoryStream(std::vector<std::byte>&& Buffer) noexcept;

        void WriteBytes(std::span<const std::byte> Src) override;

        void ReadBytes(std::span<std::byte> Dst) override;

        void Seek(ptrdiff_t Position, SeekPosition SeekFrom) override;

        size_t Tell() const override;

        size_t Size() const override;

        const std::vector<std::byte>& GetBuffer() const noexcept;

        std::vector<std::byte>& GetBuffer() noexcept;

    private:
        std::vector<std::byte> Buffer;
        size_t Position;
    };
}