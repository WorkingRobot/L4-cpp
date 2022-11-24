#pragma once

#include <cstddef>
#include <cstdint>

namespace L4::Archive
{
    static constexpr uint32_t ExpectedMagic = 0x6D69541A;
    static constexpr uint32_t DefaultSectorSize = 1 << 16;

    enum class Version : uint32_t
    {
        Unknown,
        Initial,
        Environment,
        UseIds,
        NewMagic,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };
}