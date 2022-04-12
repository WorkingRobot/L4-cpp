#pragma once

#include "StreamRun.h"

namespace L4
{
    struct StreamRunlist
    {
        uint32_t RunCount;
        // TODO:
        // uint32_t Reserved; ?
        // size_t Size;
        std::byte Reserved[12];
        StreamRun Runs[255];
    };

    static_assert(sizeof(StreamRunlist) == 4096);
    static_assert(std::has_unique_object_representations_v<StreamRunlist>);
}