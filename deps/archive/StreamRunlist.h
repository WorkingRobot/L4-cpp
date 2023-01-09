#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct StreamRunlist
    {
        struct Run
        {
            uint32_t StreamSectorOffset;
            uint32_t SectorCount;
            uint32_t SectorOffset;
            std::byte Reserved[4];
        };

        uint32_t RunCount;
        std::byte Reserved[4];
        uint64_t Size;
        Run Runs[1023];
    };

    static_assert(sizeof(StreamRunlist::Run) == 16);
    static_assert(sizeof(StreamRunlist) == 16384);
}