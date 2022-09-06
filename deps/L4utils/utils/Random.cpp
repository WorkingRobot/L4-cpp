#include "Random.h"

namespace L4
{
    // Split into different compilation unit so there aren't multiple random devices (one static for all)
    static std::mt19937 RandGenerator { std::random_device {}() };

    std::mt19937& GetRandGenerator()
    {
        return RandGenerator;
    }

    Guid RandomGuid()
    {
        return { static_cast<uint32_t>(RandGenerator()), static_cast<uint32_t>(RandGenerator()), static_cast<uint32_t>(RandGenerator()), static_cast<uint32_t>(RandGenerator()) };
    }
}