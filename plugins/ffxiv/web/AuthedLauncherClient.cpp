#include "AuthedLauncherClient.h"

namespace L4::Plugin::FFXIV
{
    AuthedLauncherClient::AuthedLauncherClient(const Models::LoginResult& AuthData) :
        AuthData(AuthData)
    {

    }
}