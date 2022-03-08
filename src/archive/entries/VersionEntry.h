#pragma once

#include "../Entry.h"

namespace L4
{
    struct VersionEntry : public EntryNonResidentSector
    {
    };

    struct VersionEntryData
    {
        uint64_t SourceVersion;
        uint64_t AppVersion;
        uint8_t Flags;
        char SourceName[64];
        char SourceVersionName[64];
        char AppName[64];
        char AppVersionName[64];
    };

    template<>
    struct EntryTraits<VersionEntry> : public EntryTraitsBase<VersionEntry>
    {
        static constexpr uint8_t Id = 0x05;
    };
}