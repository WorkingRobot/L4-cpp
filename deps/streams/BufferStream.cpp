#include "BufferStream.h"

namespace L4
{
    BufferStream::BufferStream(std::span<std::byte> Data) noexcept :
        Data(Data),
        Position(0)
    {
    }

    void BufferStream::WriteBytes(std::span<const std::byte> Src)
    {
        if (Position + Src.size() > Data.size())
        {
            throw std::runtime_error("Could not fully satisfy write");
        }
        std::ranges::copy(Src, Data.subspan(Position, Src.size()).begin());
        Position += Src.size();
    }

    void BufferStream::ReadBytes(std::span<std::byte> Dst)
    {
        if (Position + Dst.size() > Data.size())
        {
            throw std::runtime_error("Could not fully satisfy read");
        }
        std::ranges::copy(Data.subspan(Position, Dst.size()), Dst.begin());
    }

    void BufferStream::Seek(ptrdiff_t Position, SeekPosition SeekFrom)
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
            this->Position = Data.size() + Position;
            break;
        }
    }

    size_t BufferStream::Tell() const
    {
        return Position;
    }

    size_t BufferStream::Size() const
    {
        return Data.size();
    }

    std::span<const std::byte> BufferStream::GetBuffer() const noexcept
    {
        return Data;
    }
}