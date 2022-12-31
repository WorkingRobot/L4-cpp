#pragma once

#include <type_traits>

namespace L4
{
    namespace Detail
    {
        static constexpr __forceinline void HashCombine(size_t& Seed)
        {
        }

        // https://stackoverflow.com/a/38140932
        template <typename T, typename... RestTs>
        static constexpr __forceinline void HashCombine(size_t& Seed, const T& v, RestTs... Rest)
        {
            Seed ^= std::hash<T> {}(v) + 0x9E3779B9 + (Seed << 6) + (Seed >> 2);
            HashCombine(Seed, Rest...);
        }
    }

    template <typename... Ts>
    static constexpr __forceinline size_t HashCombine(Ts... Rest)
    {
        size_t Seed = 0;
        Detail::HashCombine(Seed, Rest...);
        return Seed;
    }
}