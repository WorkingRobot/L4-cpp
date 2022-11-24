#pragma once

#include <libL4/Interface.h>
#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Plugin::Manager
{
    class Auth
    {
    public:
        Auth(libL4::PluginInterface Interface);

        void OnUserUpdated();

        const libL4::Marshal::UserIdentity& GetUser();

    private:
        libL4::Marshal::UserIdentity GetUserInternal();

        bool ShouldRefreshUser;
        libL4::Marshal::UserIdentity User;

        libL4::PluginInterface Interface;
    };
}