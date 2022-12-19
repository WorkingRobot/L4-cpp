#pragma once

#include <streams/Stream.h>
#include <utils/Crc32.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    class SubStream : public Stream
    {
    public:
        SubStream(L4::Stream& Stream, size_t Offset, size_t Size) :
            Stream(Stream),
            SubOffset(Offset),
            SubSize(Size)
        {
            if (Stream.Size() < Offset + Size)
            {
                throw std::invalid_argument("Base stream doesn't have enough space for substream");
            }
        }

        void WriteBytes(std::span<const std::byte> Src) override
        {
            if (Size() - Tell() < Src.size()) [[unlikely]]
            {
                throw std::invalid_argument("Writing will cause a substream buffer overrun");
            }
            Stream.WriteBytes(Src);
        }

        void ReadBytes(std::span<std::byte> Dst) override
        {
            if (Size() - Tell() < Dst.size()) [[unlikely]]
            {
                throw std::invalid_argument("Reading will cause a substream buffer overrun");
            }
            Stream.ReadBytes(Dst);
        }

        void Seek(ptrdiff_t Position, SeekPosition SeekFrom) override
        {
            ptrdiff_t NewPosition = 0;
            switch (SeekFrom)
            {
            case Stream::Beg:
                NewPosition = Position;
                break;
            case Stream::Cur:
                NewPosition = Tell() + Position;
                break;
            case Stream::End:
                NewPosition = Size() + Position;
                break;
            default:
                throw std::invalid_argument("Invalid SeekFrom type");
            }
            if (0 > NewPosition || NewPosition > Size()) [[unlikely]]
            {
                throw std::invalid_argument("Invalid seek position");
            }
            Stream.Seek(SubOffset + NewPosition, Stream::Beg);
        }

        size_t Tell() const override
        {
            return Stream.Tell() - SubOffset;
        }

        size_t Size() const override
        {
            return SubSize;
        }

    private:
        L4::Stream& Stream;

        size_t SubOffset;
        size_t SubSize;
    };
}