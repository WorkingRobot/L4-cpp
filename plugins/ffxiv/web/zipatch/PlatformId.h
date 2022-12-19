#pragma once

#include <cstdint>

namespace L4::Plugin::FFXIV::ZiPatch
{
    enum class PlatformId : uint16_t
    {
        Win32 = 0,
        PS3 = 1,
        PS4 = 2,
        Unknown = 3,
    };
}