#pragma once

#include <streams/Stream.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    template <class T>
    struct BE
    {
        T Value;

        operator T() const noexcept
        {
            return Value;
        }
    };

    template <class T>
    struct ::L4::Serializer<BE<T>>
    {
        using BE = Plugin::FFXIV::ZiPatch::BE<T>;

        static void Serialize(Stream& Stream, const BE Val)
        {
            Stream << std::byteswap(Val.Value);
        }

        static void Deserialize(Stream& Stream, BE& Val)
        {
            Stream >> Val.Value;
            if constexpr (std::is_enum_v<T>)
            {
                Val.Value = T(std::byteswap(std::to_underlying(Val.Value)));
            }
            else
            {
                Val.Value = std::byteswap(Val.Value);
            }
        }
    };
}