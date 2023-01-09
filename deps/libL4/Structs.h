#pragma once

#include "Archive.h"
#include "Auth.h"
#include "Update.h"

namespace libL4
{
    struct PluginInterface
    {
        PluginIdentity Identity;

        AuthCallbacks Auth;

        UpdateCallbacks Update;
    };

    struct ClientInterface
    {
        InterfaceVersion InterfaceVersion;
        ClientIdentity Identity;
        String ConfigBaseDirectory;

        AuthOperations Auth;

        ArchiveOperations Archive;

        StreamOperations Stream;

        UpdateOperations Update;
    };
}
