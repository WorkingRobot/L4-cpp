#include "Stream.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    Stream::Stream(Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    StreamIdentity Stream::GetIdentity() const
    {
        libL4::StreamIdentity OutIdentity {};
        IPlugin::Instance->Client.Interface.Stream.GetIdentity(ClientHandle, &OutIdentity);
        return To(OutIdentity);
    }

    void Stream::SetIdentity(const StreamIdentity& NewIdentity)
    {
        auto MarshalledIdentity = To(NewIdentity);
        IPlugin::Instance->Client.Interface.Stream.SetIdentity(ClientHandle, &MarshalledIdentity);
    }

    uint32_t Stream::GetElementSize() const
    {
        return IPlugin::Instance->Client.Interface.Stream.GetElementSize(ClientHandle);
    }

    void Stream::SetElementSize(uint32_t NewElementSize)
    {
        IPlugin::Instance->Client.Interface.Stream.SetElementSize(ClientHandle, NewElementSize);
    }

    void Stream::ReadContext(std::span<std::byte> Dst) const
    {
        IPlugin::Instance->Client.Interface.Stream.ReadContext(ClientHandle, Dst.data(), Dst.size());
    }

    void Stream::WriteContext(std::span<const std::byte> Src)
    {
        IPlugin::Instance->Client.Interface.Stream.WriteContext(ClientHandle, Src.data(), Src.size());
    }

    uint64_t Stream::GetCapacity() const
    {
        return IPlugin::Instance->Client.Interface.Stream.GetCapacity(ClientHandle);
    }

    uint64_t Stream::GetSize() const
    {
        return IPlugin::Instance->Client.Interface.Stream.GetSize(ClientHandle);
    }

    void Stream::ReserveCapacity(uint64_t NewCapacity)
    {
        IPlugin::Instance->Client.Interface.Stream.ReserveCapacity(ClientHandle, NewCapacity);
    }

    void Stream::ShrinkToFit()
    {
        IPlugin::Instance->Client.Interface.Stream.ShrinkToFit(ClientHandle);
    }

    void Stream::Resize(uint64_t NewSize)
    {
        IPlugin::Instance->Client.Interface.Stream.Resize(ClientHandle, NewSize);
    }

    uint64_t Stream::ReadBytes(std::span<std::byte> Dst, uint64_t Offset) const
    {
        return IPlugin::Instance->Client.Interface.Stream.ReadBytes(ClientHandle, Dst.data(), Dst.size(), Offset);
    }

    uint64_t Stream::WriteBytes(std::span<const std::byte> Src, uint64_t Offset)
    {
        return IPlugin::Instance->Client.Interface.Stream.WriteBytes(ClientHandle, Src.data(), Src.size(), Offset);
    }
}