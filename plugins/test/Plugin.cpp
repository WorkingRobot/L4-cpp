#include "Plugin.h"

#include "Auth.h"
#include "Update.h"

namespace L4::Plugin::Test
{
    Plugin::Plugin(libL4::Handle ClientHandle, const libL4::ClientInterface* Interface) :
        IPlugin(ClientHandle, Interface, std::make_unique<Test::Auth>(), std::make_unique<Test::Update>())
    {
    }

    libL4::Marshal::PluginIdentity Plugin::GetIdentity() const
    {
        return {
            .Id = u8"test",
            .Name = u8"Test Plugin",
            .Version = u8"0.0.1",
            .VersionNumeric = 0
        };
    }
}