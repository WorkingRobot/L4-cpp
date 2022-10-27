#include "Archive.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    Archive::Archive(Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    ArchiveIdentity Archive::GetIdentity() const
    {
        libL4::ArchiveIdentity OutIdentity {};
        IPlugin::Instance->Client.Interface.Archive.GetIdentity(ClientHandle, &OutIdentity);
        return To(OutIdentity);
    }

    void Archive::SetIdentity(const ArchiveIdentity& NewIdentity)
    {
        auto MarshalledIdentity = To(NewIdentity);
        IPlugin::Instance->Client.Interface.Archive.SetIdentity(ClientHandle, &MarshalledIdentity);
    }

    uint32_t Archive::GetStreamCount() const
    {
        return IPlugin::Instance->Client.Interface.Archive.GetStreamCount(ClientHandle);
    }

    uint32_t Archive::GetSectorSize() const
    {
        return IPlugin::Instance->Client.Interface.Archive.GetSectorSize(ClientHandle);
    }

    uint32_t Archive::GetStreamIdxFromId(const std::u8string& Id) const
    {
        auto MarshalledId = To(Id);
        return IPlugin::Instance->Client.Interface.Archive.GetStreamIdxFromId(ClientHandle, &MarshalledId);
    }

    const Stream Archive::OpenStreamRead(uint32_t StreamIdx) const
    {
        libL4::Handle OutStream {};
        IPlugin::Instance->Client.Interface.Archive.OpenStreamRead(ClientHandle, StreamIdx, &OutStream);
        return Stream(OutStream);
    }

    Stream Archive::OpenStreamWrite(uint32_t StreamIdx)
    {
        libL4::Handle OutStream {};
        IPlugin::Instance->Client.Interface.Archive.OpenStreamWrite(ClientHandle, StreamIdx, &OutStream);
        return Stream(OutStream);
    }

    const Stream Archive::OpenStreamRead(const std::u8string& Id) const
    {
        return OpenStreamRead(GetStreamIdxFromId(Id));
    }

    Stream Archive::OpenStreamWrite(const std::u8string& Id)
    {
        return OpenStreamWrite(GetStreamIdxFromId(Id));
    }
}