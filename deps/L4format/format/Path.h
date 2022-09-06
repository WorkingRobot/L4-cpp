#pragma once

#include "Format.h"

#include <filesystem>

template <class CharT>
struct FMT::formatter<std::filesystem::path, CharT> : FMT::formatter<std::basic_string_view<CharT>, CharT>
{
    template <class FormatContext>
    auto format(const std::filesystem::path& Path, FormatContext& Ctx) const
    {
        return FMT::formatter<std::basic_string_view<CharT>, CharT>::format(Path.string<CharT>(), Ctx);
    }
};
