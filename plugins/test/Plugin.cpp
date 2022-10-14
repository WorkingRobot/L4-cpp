#include "Plugin.h"

namespace L4::Plugin::Test
{
    Plugin::Plugin(const libL4::ClientInterface* Interface) :
        IPlugin(Interface)
    {
    }

    Wrapper::PluginIdentity Plugin::GetIdentity() const
    {
        return {
            .Id = u8"test",
            .Name = u8"Test Plugin",
            .Version = u8"0.0.1",
            .VersionNumeric = 0
        };
    }
}