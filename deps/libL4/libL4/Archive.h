#pragma once

#include "Stream.h"

namespace libL4
{
    typedef void* Archive;

    struct ArchiveIdentity
    {
        AppIdentity App;
        PluginIdentity Plugin;
        String Environment;
    };

    struct ArchiveOperations
    {
        void (*GetIdentity)(Archive Archive, ArchiveIdentity* OutIdentity);
        void (*SetIdentity)(Archive Archive, const ArchiveIdentity* NewIdentity);
        uint32_t (*GetStreamCount)(Archive Archive);
        uint32_t (*GetSectorSize)(Archive Archive);
        uint32_t (*GetStreamIdxFromId)(Archive Archive, const String* Id);
        void (*OpenStreamRead)(Archive Archive, uint32_t StreamIdx, Stream* OutStream);
        void (*OpenStreamWrite)(Archive Archive, uint32_t StreamIdx, Stream* OutStream);
        void (*CloseStream)(Archive Archive, Stream Stream);
    };
}