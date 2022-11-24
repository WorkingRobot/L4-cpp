#include "Auth.h"

namespace L4::Plugin::Manager
{
    Auth::Auth(libL4::PluginInterface Interface) :
        Interface(Interface),
        ShouldRefreshUser(true)
    {
    }

    void Auth::OnUserUpdated()
    {
        ShouldRefreshUser = true;
    }

    const libL4::Marshal::UserIdentity& Auth::GetUser()
    {
        if (ShouldRefreshUser)
        {
            User = GetUserInternal();
        }
        return User;
    }

    libL4::Marshal::UserIdentity Auth::GetUserInternal()
    {
        libL4::UserIdentity Identity;
        Interface.Auth.GetUser(&Identity);

        return libL4::Marshal::To(Identity);
    }
}