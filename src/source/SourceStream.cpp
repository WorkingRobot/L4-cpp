#include "SourceStream.h"

#include "../archive/StreamView.h"

namespace L4
{
    SourceStream::SourceStream(StreamViewWritable Stream) :
        Stream(Stream)
    {
    }

    Source::StreamIdentity SourceStream::GetIdentity() const
    {
        auto& Header = Stream.GetHeader();

        return Source::StreamIdentity {
            .Id = SerializeString(Header.Id),
            .Name = SerializeString(GetSV(Header.Name)),
            .Version = Header.Version
        };
    }

    void SourceStream::SetIdentity(const Source::StreamIdentity& NewIdentity)
    {
        auto& Header = Stream.GetHeader();

        SetSV(Header.Id, DeserializeString(NewIdentity.Id));
        SetSV(Header.Name, DeserializeString(NewIdentity.Name));
        Header.Version = NewIdentity.Version;
    }

    uint32_t SourceStream::GetElementSize() const
    {
        return Stream.GetElementSize();
    }

    void SourceStream::SetElementSize(uint32_t NewElementSize)
    {
        Stream.GetHeader().ElementSize = NewElementSize;
    }

    std::span<const std::byte, 184> SourceStream::GetContext() const
    {
        return Stream.GetContext();
    }

    void SourceStream::SetContext(std::span<const std::byte, 184> NewContext)
    {
        std::ranges::copy(NewContext, Stream.GetHeader().Context);
    }

    uint64_t SourceStream::GetCapacity() const
    {
        return Stream.GetCapacity();
    }

    uint64_t SourceStream::GetSize() const
    {
        return Stream.GetRunlist().Size;
    }

    void SourceStream::ReserveCapacity(uint64_t NewCapacity)
    {
        return Stream.Reserve(NewCapacity);
    }

    void SourceStream::ShrinkToFit()
    {
        return Stream.ShrinkToFit();
    }

    void SourceStream::Resize(uint64_t NewSize)
    {
        return Stream.Resize(NewSize);
    }

    uint64_t SourceStream::Read(std::span<std::byte> Dst, uint64_t Offset) const
    {
        auto Size = GetSize();
        if (Offset >= Size)
        {
            return 0;
        }
        if (Offset + Dst.size() > Size)
        {
            Dst = Dst.first(Size - Offset);
        }
        Stream.Read(Dst, Offset);
        return Dst.size();
    }

    uint64_t SourceStream::Write(std::span<const std::byte> Src, uint64_t Offset)
    {
        auto Size = GetSize();
        if (Offset >= Size)
        {
            return 0;
        }
        if (Offset + Src.size() > Size)
        {
            Src = Src.first(Size - Offset);
        }
        Stream.Write(Src, Offset);
        return Src.size();
    }
}