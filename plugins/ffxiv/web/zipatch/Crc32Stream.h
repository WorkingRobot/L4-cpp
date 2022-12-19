#pragma once

#include <streams/Stream.h>
#include <utils/Crc32.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    class Crc32Stream : public Stream
    {
    public:
        explicit Crc32Stream(L4::Stream& Stream) :
            Stream(Stream),
            Checksum(~0u)
        {
        }

        void WriteBytes(std::span<const std::byte> Src) override
        {
            Stream.WriteBytes(Src);
            UpdateChecksum(Src);
        }

        void ReadBytes(std::span<std::byte> Dst) override
        {
            Stream.ReadBytes(Dst);
            UpdateChecksum(Dst);
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
            auto PositionDelta = NewPosition - ptrdiff_t(Tell());
            if (PositionDelta < 0) [[unlikely]]
            {
                throw std::invalid_argument("Cannot seek backwards in a Crc32 stream");
            }

            auto Buffer = std::make_unique<std::byte[]>(PositionDelta);
            ReadBytes(std::span(Buffer.get(), PositionDelta));
        }

        size_t Tell() const override
        {
            return Stream.Tell();
        }

        size_t Size() const override
        {
            return Stream.Size();
        }

        uint32_t GetChecksum() const noexcept
        {
            return ~Checksum;
        }

        void ClearChecksum() noexcept
        {
            Checksum = ~0u;
        }

    private:
        L4::Stream& Stream;

        void UpdateChecksum(std::span<const std::byte> Data)
        {
            Checksum = Detail::Crc32LoopIntrinsics(Checksum, Data);
        }

        uint32_t Checksum;
    };
}