#include "Archive.h"

#include <algorithm>

namespace L4::Manager
{
    static libL4::Marshal::ArchiveIdentity ToIdentity(const L4::Archive::Header& Header)
    {
        return {
            .App = {

                .Id = libL4::Marshal::String(L4::Archive::ToString(Header.AppId)),
                .Name = libL4::Marshal::String(L4::Archive::ToString(Header.AppName)),
                .Version = libL4::Marshal::String(L4::Archive::ToString(Header.AppVersion)),
                .VersionNumeric = Header.AppVersionNumeric,
            },
            .Plugin = {
                .Id = libL4::Marshal::String(L4::Archive::ToString(Header.PluginId)),
                .Name = libL4::Marshal::String(L4::Archive::ToString(Header.PluginName)),
                .Version = libL4::Marshal::String(L4::Archive::ToString(Header.PluginVersion)),
                .VersionNumeric = Header.PluginVersionNumeric,
            },
            .Environment = libL4::Marshal::String(L4::Archive::ToString(Header.Environment))
        };
    }

    Archive::Archive(const std::filesystem::path& Path, std::false_type IsWritable) :
        Impl(std::in_place_type<L4::Archive::Archive>, Path)
    {
    }

    Archive::Archive(const std::filesystem::path& Path, std::true_type IsWritable) :
        Impl(std::in_place_type<L4::Archive::ArchiveWritable>, Path)
    {
    }

    libL4::Marshal::ArchiveIdentity Archive::GetIdentity() const
    {
        return std::visit([](const auto& Ar) { return ToIdentity(Ar.GetHeader()); }, Impl);
    }

    void Archive::SetIdentity(const libL4::Marshal::ArchiveIdentity& NewIdentity)
    {
        if (auto* Ar = std::get_if<L4::Archive::ArchiveWritable>(&Impl)) [[likely]]
        {
            // TODO: this suffers from ABA problem
            // However, this shouldn't really be called all the time.
            auto Header = Ar->GetHeader();
            Header.PluginVersionNumeric = NewIdentity.Plugin.VersionNumeric;
            Header.AppVersionNumeric = NewIdentity.App.VersionNumeric;
            L4::Archive::FromString(NewIdentity.Plugin.Id, Header.PluginId);
            L4::Archive::FromString(NewIdentity.App.Id, Header.AppId);
            L4::Archive::FromString(NewIdentity.Plugin.Name, Header.PluginName);
            L4::Archive::FromString(NewIdentity.App.Name, Header.AppName);
            L4::Archive::FromString(NewIdentity.Plugin.Version, Header.PluginVersion);
            L4::Archive::FromString(NewIdentity.App.Version, Header.AppVersion);
            L4::Archive::FromString(NewIdentity.Environment, Header.Environment);
            return Ar->SetHeaderMeta(Header);
        }
        else
        {
            throw std::invalid_argument("Archive is read only.");
        }
    }

    uint32_t Archive::GetStreamCount() const
    {
        return std::visit([](const auto& Ar) { return Ar.GetHeader().StreamCount; }, Impl);
    }

    uint32_t Archive::GetSectorSize() const
    {
        return std::visit([](const auto& Ar) { return Ar.GetHeader().SectorSize; }, Impl);
    }

    Stream Archive::OpenStream(std::u8string_view Id)
    {
        return std::visit([Id](auto& Archive) { return Stream(Archive.GetStream(Id)); }, Impl);
    }

    libL4::Handle Archive::OpenStreamInternal(std::u8string_view Id)
    {
        auto& Stream = Streams.emplace_front(OpenStream(Id));
        return (libL4::Handle)&Stream;
    }

    bool Archive::CloseStreamInternal(libL4::Handle Handle)
    {
        return Streams.remove_if([Handle](const auto& Stream) { return (libL4::Handle)&Stream == Handle; }) == 1;
    }
}