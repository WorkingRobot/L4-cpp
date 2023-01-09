#include "Stream.h"

namespace L4::Manager
{
    static libL4::Marshal::StreamIdentity ToIdentity(const Archive::StreamHeader& Header)
    {
        return {
            .Id = libL4::Marshal::String(Archive::ToString(Header.Id))
        };
    }

    Stream::Stream(Archive::Stream&& Stream) :
        Impl(std::move(Stream))
    {
    }

    Stream::Stream(Archive::StreamWritable&& Stream) :
        Impl(std::move(Stream))
    {
    }

    libL4::Marshal::StreamIdentity Stream::GetIdentity() const
    {
        return std::visit([](const auto& Stream) { return ToIdentity(Stream.GetHeader()); }, Impl);
    }

    void Stream::SetIdentity(const libL4::Marshal::StreamIdentity& NewIdentity)
    {
        if (auto* Stream = std::get_if<Archive::StreamWritable>(&Impl)) [[likely]]
        {
            // TODO: this suffers from ABA problem
            // However, this shouldn't really be called all the time.
            auto Header = Stream->GetHeader();
            Archive::FromString(NewIdentity.Id, Header.Id);
            return Stream->SetHeader(Header);
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }

    void Stream::ReadContext(std::span<std::byte, sizeof(L4::Archive::StreamHeader::Context)> Dst) const
    {
        std::visit([Dst](const auto& Stream) { std::ranges::copy(Stream.GetHeader().Context, Dst.begin()); }, Impl);
    }

    void Stream::WriteContext(std::span<const std::byte, sizeof(L4::Archive::StreamHeader::Context)> Src)
    {
        if (auto* Stream = std::get_if<L4::Archive::StreamWritable>(&Impl)) [[likely]]
        {
            // TODO: this suffers from ABA problem
            // However, this shouldn't really be called all the time.
            auto Header = Stream->GetHeader();
            std::ranges::copy(Src, Header.Context);
            return Stream->SetHeader(Header);
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }

    uint64_t Stream::GetCapacity() const
    {
        return std::visit([](const auto& Stream) { return Stream.GetCapacity(); }, Impl);
    }

    uint64_t Stream::GetSize() const
    {
        return std::visit([](const auto& Stream) { return Stream.GetSize(); }, Impl);
    }

    void Stream::ReserveCapacity(uint64_t NewCapacity)
    {
        if (auto* Stream = std::get_if<L4::Archive::StreamWritable>(&Impl)) [[likely]]
        {
            Stream->ReserveCapacity(NewCapacity);
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }

    void Stream::ShrinkToFit()
    {
        if (auto* Stream = std::get_if<L4::Archive::StreamWritable>(&Impl)) [[likely]]
        {
            Stream->ShrinkToFit();
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }

    void Stream::Resize(uint64_t NewSize)
    {
        if (auto* Stream = std::get_if<L4::Archive::StreamWritable>(&Impl)) [[likely]]
        {
            Stream->Resize(NewSize);
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }

    void Stream::ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const
    {
        std::visit([Offset, Dst](const auto& Stream) { Stream.ReadBytes(Offset, Dst); }, Impl);
    }

    void Stream::WriteBytes(std::span<const std::byte> Src, uint64_t Offset)
    {
        if (auto* Stream = std::get_if<L4::Archive::StreamWritable>(&Impl)) [[likely]]
        {
            Stream->WriteBytes(Offset, Src);
        }
        else
        {
            throw std::invalid_argument("Stream is read only.");
        }
    }
}