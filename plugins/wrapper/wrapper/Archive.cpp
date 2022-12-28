#include "Archive.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    Archive::Archive(libL4::Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    libL4::Marshal::ArchiveIdentity Archive::GetIdentity() const
    {
        libL4::ArchiveIdentity OutIdentity {};
        IPlugin::Instance->Client->Archive.GetIdentity(ClientHandle, &OutIdentity);
        return libL4::Marshal::To(OutIdentity);
    }

    void Archive::SetIdentity(const libL4::Marshal::ArchiveIdentity& NewIdentity)
    {
        auto MarshalledIdentity = libL4::Marshal::To(NewIdentity);
        IPlugin::Instance->Client->Archive.SetIdentity(ClientHandle, &MarshalledIdentity);
    }

    uint32_t Archive::GetStreamCount() const
    {
        return IPlugin::Instance->Client->Archive.GetStreamCount(ClientHandle);
    }

    uint32_t Archive::GetSectorSize() const
    {
        return IPlugin::Instance->Client->Archive.GetSectorSize(ClientHandle);
    }

    uint32_t Archive::GetStreamIdxFromId(const std::u8string& Id) const
    {
        auto MarshalledId = libL4::Marshal::To(Id);
        return IPlugin::Instance->Client->Archive.GetStreamIdxFromId(ClientHandle, &MarshalledId);
    }

    const Stream Archive::OpenStreamRead(uint32_t StreamIdx) const
    {
        libL4::Handle OutStream {};
        IPlugin::Instance->Client->Archive.OpenStreamRead(ClientHandle, StreamIdx, &OutStream);
        return Stream(OutStream);
    }

    Stream Archive::OpenStreamWrite(uint32_t StreamIdx)
    {
        libL4::Handle OutStream {};
        IPlugin::Instance->Client->Archive.OpenStreamWrite(ClientHandle, StreamIdx, &OutStream);
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