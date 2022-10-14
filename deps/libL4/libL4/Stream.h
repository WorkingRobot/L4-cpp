#pragma once

#include "Utils.h"

namespace libL4
{
    struct StreamIdentity
    {
        String Id;
        String Name;
        uint32_t VersionNumeric;
    };

    struct StreamOperations
    {
        void (*GetIdentity)(Handle Stream, StreamIdentity* OutIdentity);
        void (*SetIdentity)(Handle Stream, const StreamIdentity* NewIdentity);
        uint32_t (*GetElementSize)(Handle Stream);
        void (*SetElementSize)(Handle Stream, uint32_t NewElementSize);
        void (*ReadContext)(Handle Stream, void* Dst, uint32_t Size);
        void (*WriteContext)(Handle Stream, const void* Src, uint32_t Size);
        uint64_t (*GetCapacity)(Handle Stream);
        uint64_t (*GetSize)(Handle Stream);
        void (*ReserveCapacity)(Handle Stream, uint64_t NewCapacity);
        void (*ShrinkToFit)(Handle Stream);
        void (*Resize)(Handle Stream, uint64_t NewSize);
        uint64_t (*ReadBytes)(Handle Stream, void* Dst, uint64_t Size, uint64_t Offset);
        uint64_t (*WriteBytes)(Handle Stream, const void* Src, uint64_t Size, uint64_t Offset);
    };
}