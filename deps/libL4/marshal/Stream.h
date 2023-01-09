#pragma once

#include "Utils.h"

namespace libL4::Marshal
{
    struct StreamIdentity
    {
        String Id;
    };

    L4_MARSHAL_BETWEEN(StreamIdentity, Id)
}