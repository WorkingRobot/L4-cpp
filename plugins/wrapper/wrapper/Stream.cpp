#include "Stream.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    Stream::Stream(libL4::Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    libL4::Marshal::StreamIdentity Stream::GetIdentity() const
    {
        libL4::StreamIdentity OutIdentity {};
        IPlugin::Instance->Client->Stream.GetIdentity(ClientHandle, &OutIdentity);
        return libL4::Marshal::To(OutIdentity);
    }

    void Stream::SetIdentity(const libL4::Marshal::StreamIdentity& NewIdentity)
    {
        auto MarshalledIdentity = libL4::Marshal::To(NewIdentity);
        IPlugin::Instance->Client->Stream.SetIdentity(ClientHandle, &MarshalledIdentity);
    }

    uint32_t Stream::GetElementSize() const
    {
        return IPlugin::Instance->Client->Stream.GetElementSize(ClientHandle);
    }

    void Stream::SetElementSize(uint32_t NewElementSize)
    {
        IPlugin::Instance->Client->Stream.SetElementSize(ClientHandle, NewElementSize);
    }

    void Stream::ReadContext(std::span<std::byte> Dst) const
    {
        IPlugin::Instance->Client->Stream.ReadContext(ClientHandle, Dst.data(), Dst.size());
    }

    void Stream::WriteContext(std::span<const std::byte> Src)
    {
        IPlugin::Instance->Client->Stream.WriteContext(ClientHandle, Src.data(), Src.size());
    }

    uint64_t Stream::GetCapacity() const
    {
        return IPlugin::Instance->Client->Stream.GetCapacity(ClientHandle);
    }

    uint64_t Stream::GetSize() const
    {
        return IPlugin::Instance->Client->Stream.GetSize(ClientHandle);
    }

    void Stream::ReserveCapacity(uint64_t NewCapacity)
    {
        IPlugin::Instance->Client->Stream.ReserveCapacity(ClientHandle, NewCapacity);
    }

    void Stream::ShrinkToFit()
    {
        IPlugin::Instance->Client->Stream.ShrinkToFit(ClientHandle);
    }

    void Stream::Resize(uint64_t NewSize)
    {
        IPlugin::Instance->Client->Stream.Resize(ClientHandle, NewSize);
    }

    uint64_t Stream::ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const
    {
        return IPlugin::Instance->Client->Stream.ReadBytes(ClientHandle, Dst.data(), Dst.size(), Offset);
    }

    uint64_t Stream::WriteBytes(std::span<const std::byte> Src, uint64_t Offset)
    {
        return IPlugin::Instance->Client->Stream.WriteBytes(ClientHandle, Src.data(), Src.size(), Offset);
    }
}