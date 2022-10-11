#pragma once

#include "../Structs.h"

#include "Macros.h"

#include <concepts>
#include <string>

namespace libL4::Marshal
{
    bool To(bool In)
    {
        return In;
    }

    uint32_t To(uint32_t In)
    {
        return In;
    }

    uint64_t To(uint64_t In)
    {
        return In;
    }

    using String = std::u8string;

    libL4::String To(const String& In)
    {
        return {
            .Data = In.c_str(),
            .Size = In.size()
        };
    }

    String To(const libL4::String& In)
    {
        return String(In.Data, In.Size);
    }

    struct AppIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    L4_MARSHAL_BETWEEN(AppIdentity, Id, Name, Version, VersionNumeric)

    struct PluginIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    L4_MARSHAL_BETWEEN(PluginIdentity, Id, Name, Version, VersionNumeric)

    struct ClientIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    L4_MARSHAL_BETWEEN(ClientIdentity, Id, Name, Version, VersionNumeric)
}