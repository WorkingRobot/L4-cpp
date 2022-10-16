#include "Plugin.h"

#include "Auth.h"
#include "Update.h"

namespace L4::Plugin::Test
{
    Plugin::Plugin(const libL4::ClientInterface* Interface) :
        IPlugin(Interface, std::make_unique<Test::Auth>(), std::make_unique<Test::Update>())
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