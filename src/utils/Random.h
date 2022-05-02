#pragma once

#include <random>

namespace L4
{
    std::mt19937& GetRandGenerator();

    void RandomGuid(char Guid[16]);

    template <class T>
    static T Random(T Min = std::numeric_limits<T>::min(), T Max = std::numeric_limits<T>::max())
    {
        return std::uniform_int_distribution<T> { Min, Max }(GetRandGenerator());
    }

    template <class Iter>
    static Iter RandomChoice(Iter Start, Iter End)
    {
        std::advance(Start, Random(0, std::distance(Start, End) - 1));
        return Start;
    }
}