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

    Stream Archive::OpenStream(const std::u8string& Id)
    {
        auto MarshalledId = libL4::Marshal::To(Id);
        libL4::Handle OutStream {};
        IPlugin::Instance->Client->Archive.OpenStream(ClientHandle, &MarshalledId, &OutStream);
        return Stream(OutStream);
    }
}