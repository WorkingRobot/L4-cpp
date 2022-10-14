#pragma once

#include <cstdint>

namespace libL4
{
    typedef uintptr_t Handle;

    struct String
    {
        uint8_t Size;
        char8_t Data[255];
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