#pragma once

#include <stdint.h>

namespace L4
{
    static constexpr uint32_t ExpectedMagic = 0xCAE791A5;
    static constexpr uint32_t DefaultSectorSize = 1 << 16;

    enum class ArchiveVersion : uint32_t
    {
        Unknown,
        Initial,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };
}