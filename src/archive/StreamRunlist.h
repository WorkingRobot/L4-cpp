#pragma once

#include "StreamRun.h"

namespace L4
{
    struct StreamRunlist
    {
        uint32_t RunCount;
        uint32_t Reserved;
        uint64_t Size;
        StreamRun Runs[255];
    };

    static_assert(sizeof(StreamRunlist) == 4096);
    static_assert(std::has_unique_object_representations_v<StreamRunlist>);
}