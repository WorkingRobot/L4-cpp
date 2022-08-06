#pragma once

#include <L4/source/Structs.h>

#include <string_view>

namespace L4
{
    static Source::String SerializeString(const std::u8string_view View)
    {
        return Source::String {
            .Data = View.data(),
            .Size = View.size()
        };
    }

    static std::u8string_view DeserializeString(const Source::String String)
    {
        return std::u8string_view(String.Data, String.Size);
    }
}
