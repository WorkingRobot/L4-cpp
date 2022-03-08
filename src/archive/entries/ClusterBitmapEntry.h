#pragma once

#include "../Entry.h"

namespace L4
{
    struct ClusterBitmapEntry : public EntryNonResidentCluster
    {
        
    };

    template<>
    struct EntryTraits<ClusterBitmapEntry> : public EntryTraitsBase<ClusterBitmapEntry>
    {
        static constexpr uint8_t Id = 0x01;
    };
}