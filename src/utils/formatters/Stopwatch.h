#pragma once

#include "../Stopwatch.h"

#include <format>

template <class CharT>
struct std::formatter<L4::Stopwatch, CharT> : std::formatter<float, CharT>
{
    template <class FormatContext>
    auto format(const L4::Stopwatch& Stopwatch, FormatContext& Ctx) const
    {
        return std::formatter<float, CharT>::format(Stopwatch.TimeElapsedMs(), Ctx);
    }
};