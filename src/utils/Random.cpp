#include "Random.h"

namespace L4 {
    // Split into different compilation unit so there aren't multiple random devices (one static for all)
    static std::mt19937 RandGenerator { std::random_device {}() };

    std::mt19937& GetRandGenerator()
    {
        return RandGenerator;
    }

    void RandomGuid(char Guid[16])
    {
        *(uint32_t*)(Guid + 0) = RandGenerator();
        *(uint32_t*)(Guid + 4) = RandGenerator();
        *(uint32_t*)(Guid + 8) = RandGenerator();
        *(uint32_t*)(Guid + 12) = RandGenerator();
    }
}