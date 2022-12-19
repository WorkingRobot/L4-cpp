#pragma once

#include "models/LoginResult.h"

#include <web/Client.h>

namespace L4::Plugin::FFXIV
{
    class AuthedLauncherClient : public Web::Http::Client
    {
    public:
        AuthedLauncherClient(const Models::LoginResult& AuthData);

    private:
        Models::LoginResult AuthData;
    };
}