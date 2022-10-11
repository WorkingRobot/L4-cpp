#pragma once

#include <cstdint>

namespace libL4
{
    struct String
    {
        const char8_t* Data;
        uint64_t Size;
    };

    struct AppIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    struct PluginIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    struct ClientIdentity
    {
        String Id;
        String Name;
        String Version;
        uint32_t VersionNumeric;
    };

    enum class InterfaceVersion : uint16_t
    {
        Unknown = 0,
        Initial,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };
}