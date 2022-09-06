#include "Random.h"

#include <random>

namespace FastSpd
{
    // Split into different compilation unit so there aren't multiple random devices (one static for all)
    // NOLINTNEXTLINE(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables, cert-err58-cpp)
    static std::mt19937 RandGenerator { std::random_device {}() };

    GUID UuidCreate()
    {
        GUID Guid;
        auto* GuidPtr = reinterpret_cast<uint32_t*>(&Guid);
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        *GuidPtr++ = RandGenerator();
        return Guid;
    }
}