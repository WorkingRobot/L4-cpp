#pragma once

#include <wrapper/IPlugin.h>

namespace L4::Plugin::Test
{
    class Plugin final : public Wrapper::IPlugin
    {
    public:
        Plugin(const libL4::ClientInterface* Interface);

        L4::Plugin::Wrapper::PluginIdentity GetIdentity() const final;
    };
}