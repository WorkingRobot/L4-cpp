#pragma once

#include <compat/expected.h>

#include <charconv>
#include <string>

namespace L4
{
    template <class T>
    static constexpr std::expected<T, std::errc> ParseNum(const std::string_view View) noexcept
    {
        T Ret {};
        std::from_chars_result Result = std::from_chars(View.data(), View.data() + View.size(), Ret);
        if (Result.ptr == View.data() + View.size()) [[likely]]
        {
            return Ret;
        }
        return std::unexpected(Result.ec != std::errc(0) ? Result.ec : std::errc::invalid_argument);
    }
}