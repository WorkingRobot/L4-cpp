#pragma once

#include "Utils.h"

namespace libL4::Marshal
{
    struct ArchiveIdentity
    {
        AppIdentity App;
        PluginIdentity Plugin;
        String Environment;
    };

    L4_MARSHAL_BETWEEN(ArchiveIdentity, App, Plugin, Environment)
}