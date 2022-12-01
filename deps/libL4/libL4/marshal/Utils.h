#pragma once

#include "../Structs.h"

#include "Macros.h"

#include <string>
#include <stdexcept>

namespace libL4::Marshal
{
    using libL4::Handle;

    static bool To(bool In)
    {
        return In;
    }

    static uint32_t To(uint32_t In)
    {
        return In;
    }

    static uint64_t To(uint64_t In)
    {
        return In;
    }

    using String = std::u8string;

    static libL4::String To(const String& In)
    {
        if (In.size() > 250)
            throw std::out_of_range("String might be too big"); // TODO: remove before deployment, this is just a failsafe
        libL4::String Ret {
            .Size = uint8_t(In.size())
        };
        In.copy(Ret.Data, Ret.Size);
        return Ret;
    }

    static String To(const libL4::String& In)
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