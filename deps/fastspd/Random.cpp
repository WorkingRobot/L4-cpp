#include "Random.h"

#include <random>

namespace FastSpd
{
    // Split into different compilation unit so there aren't multiple random devices (one static for all)
    static std::mt19937 RandGenerator { std::random_device {}() };

    GUID UuidCreate()
    {
        GUID Guid;
        auto GuidPtr = (uint32_t*)&Guid;
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        return Guid;
    }
}