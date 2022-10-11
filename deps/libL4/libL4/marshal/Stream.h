#pragma once

#include "Utils.h"

namespace libL4::Marshal
{
    struct StreamIdentity
    {
        String Id;
        String Name;
        uint32_t VersionNumeric;
    };

    L4_MARSHAL_BETWEEN(StreamIdentity, Id, Name, VersionNumeric)
}