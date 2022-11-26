#include "Plugin.h"

#include "Auth.h"
#include "Update.h"

namespace L4::Plugin::FFXIV
{
    Plugin::Plugin(const libL4::ClientInterface* Interface) :
        IPlugin(Interface, std::make_unique<FFXIV::Auth>(), std::make_unique<FFXIV::Update>())
    {
    }

    Wrapper::PluginIdentity Plugin::GetIdentity() const
    {
        return {
            .Id = u8"ffxiv",
            .Name = u8"FFXIV",
            .Version = u8"0.0.1",
            .VersionNumeric = 0
        };
    }
}