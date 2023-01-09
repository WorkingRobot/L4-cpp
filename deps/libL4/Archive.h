#pragma once

#include "Stream.h"

namespace libL4
{
    struct ArchiveIdentity
    {
        AppIdentity App;
        PluginIdentity Plugin;
        String Environment;
    };

    struct ArchiveOperations
    {
        void (*GetIdentity)(Handle Archive, ArchiveIdentity* OutIdentity);
        void (*SetIdentity)(Handle Archive, const ArchiveIdentity* NewIdentity);
        uint32_t (*GetStreamCount)(Handle Archive);
        uint32_t (*GetSectorSize)(Handle Archive);
        void (*OpenStream)(Handle Archive, const String* Id, Handle* OutStream);
        void (*CloseStream)(Handle Archive, Handle Stream);
    };
}