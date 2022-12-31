#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct Header
    {
        uint32_t Magic;
        Version Version;
        uint32_t SectorSize;
        uint32_t SequenceNumber;
        uint32_t StreamCount;
        uint32_t MaxStreamCount;
        uint32_t PluginVersionNumeric;
        uint32_t AppVersionNumeric;
        char8_t PluginId[32];
        char8_t AppId[32];
        char8_t PluginName[32];
        char8_t AppName[32];
        char8_t PluginVersion[32];
        char8_t AppVersion[32];
        char8_t Environment[32];
    };

    static_assert(sizeof(Header) == 256);
}