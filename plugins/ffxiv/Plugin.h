#pragma once

#include <wrapper/IPlugin.h>

namespace L4::Plugin::FFXIV
{
    class Plugin final : public Wrapper::IPlugin
    {
    public:
        Plugin(libL4::Handle ClientHandle, const libL4::ClientInterface* Interface);

        libL4::Marshal::PluginIdentity GetIdentity() const final;
    };
}