#pragma once

#include "../Entry.h"

namespace L4
{
    struct FileEntry : public EntrySuper
    {
        
    };

    template<>
    struct EntryTraits<FileEntry> : public EntryTraitsBase<FileEntry>
    {
        static constexpr uint8_t Id = 0x07;
    };
}