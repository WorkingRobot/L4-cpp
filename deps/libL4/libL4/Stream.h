#pragma once

#include "Utils.h"

namespace libL4
{
    typedef void* Stream;

    struct StreamIdentity
    {
        String Id;
        String Name;
        uint32_t VersionNumeric;
    };

    struct StreamOperations
    {
        void (*GetIdentity)(Stream Stream, StreamIdentity* OutIdentity);
        void (*SetIdentity)(Stream Stream, const StreamIdentity* NewIdentity);
        uint32_t (*GetElementSize)(Stream Stream);
        void (*SetElementSize)(Stream Stream, uint32_t NewElementSize);
        void (*ReadContext)(Stream Stream, void* Dst, uint32_t Size);
        void (*WriteContext)(Stream Stream, const void* Src, uint32_t Size);
        uint64_t (*GetCapacity)(Stream Stream);
        uint64_t (*GetSize)(Stream Stream);
        void (*ReserveCapacity)(Stream Stream, uint64_t NewCapacity);
        void (*ShrinkToFit)(Stream Stream);
        void (*Resize)(Stream Stream, uint64_t NewSize);
        uint64_t (*ReadBytes)(Stream Stream, void* Dst, uint64_t Size, uint64_t Offset);
        uint64_t (*WriteBytes)(Stream Stream, const void* Src, uint64_t Size, uint64_t Offset);
    };
}