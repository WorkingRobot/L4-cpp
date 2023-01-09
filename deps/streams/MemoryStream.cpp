#include "MemoryStream.h"

namespace L4
{
    MemoryStream::MemoryStream(size_t Capacity) :
        Position(0)
    {
        Buffer.reserve(Capacity);
    }

    MemoryStream::MemoryStream(std::vector<std::byte>&& Buffer) noexcept :
        Buffer(std::move(Buffer)),
        Position(0)
    {

    }

    void MemoryStream::WriteBytes(std::span<const std::byte> Src)
    {
        if (Position + Src.size() > Buffer.size())
        {
            Buffer.resize(Position + Src.size());
        }
        std::ranges::copy(Src, Buffer.begin() + Position);
        Position += Src.size();
    }

    void MemoryStream::ReadBytes(std::span<std::byte> Dst)
    {
        if (Position + Dst.size() > Buffer.size())
        {
            throw std::runtime_error("Could not fully satisfy read");
        }
        std::ranges::copy_n(Buffer.begin() + Position, Dst.size(), Dst.begin());
    }

    void MemoryStream::Seek(ptrdiff_t Position, SeekPosition SeekFrom)
    {
        switch (SeekFrom)
        {
        case Beg:
            this->Position = Position;
            break;
        case Cur:
            this->Position += Position;
            break;
        case End:
            this->Position = Buffer.size() + Position;
            break;
        }
    }

    size_t MemoryStream::Tell() const
    {
        return Position;
    }

    size_t MemoryStream::Size() const
    {
        return Buffer.size();
    }

    const std::vector<std::byte>& MemoryStream::GetBuffer() const noexcept
    {
        return Buffer;
    }

    std::vector<std::byte>& MemoryStream::GetBuffer() noexcept
    {
        return Buffer;
    }
}