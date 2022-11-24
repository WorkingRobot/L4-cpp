#pragma once

#include <cstddef>
#include <cstdint>

namespace L4::Archive
{
    static constexpr uint32_t ExpectedMagic = 0xCAE791A5;
    static constexpr uint32_t DefaultSectorSize = 1 << 16;

    enum class Version : uint32_t
    {
        Unknown,
        Initial,
        Environment,
        UseIds,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };
}