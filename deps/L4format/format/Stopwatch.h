#pragma once

#include "utils/Stopwatch.h"

template <class CharT>
struct FMT::formatter<L4::Stopwatch, CharT> : FMT::formatter<float, CharT>
{
    template <class FormatContext>
    auto format(const L4::Stopwatch& Stopwatch, FormatContext& Ctx) const
    {
        return FMT::formatter<float, CharT>::format(Stopwatch.TimeElapsedMs(), Ctx);
    }
};
#endif
