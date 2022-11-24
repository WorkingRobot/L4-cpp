#pragma once

#include "Utils.h"

namespace L4::Archive
{
    struct StreamRunlist
    {
        struct Run
        {
            uint32_t StreamSectorCount;
            uint32_t SectorOffset;
            uint32_t SectorCount;
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