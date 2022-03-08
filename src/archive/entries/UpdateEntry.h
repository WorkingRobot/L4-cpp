#pragma once

#include "../Entry.h"

namespace L4
{
    struct UpdateEntry : public EntryNonResidentSector
    {
        uint64_t MsElapsed;
    };

    struct UpdateEntryData
    {
        uint64_t ChunksCompleteHistory[16];
        uint64_t ReadPeak;
        uint64_t WritePeak;
        uint64_t DownloadPeak;
    };

    template<>
    struct EntryTraits<UpdateEntry> : public EntryTraitsBase<UpdateEntry>
    {
        static constexpr uint8_t Id = 0x06;
    };
}