#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace L4::Archive
{
    static constexpr uint32_t ExpectedMagic = 0x6D69541A;
    static constexpr uint32_t DefaultSectorSize = 1 << 16;

    enum class Version : uint32_t
    {
        Unknown,
        Initial,
        Environment,
        UseIds,
        NewMagic,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };

    template <size_t Size>
    static constexpr std::u8string_view ToString(const char8_t (&In)[Size])
    {
        size_t Idx;
        for (Idx = 0; Idx < Size; ++Idx)
        {
            if (!In[Idx])
            {
                break;
            }
        }
        return std::u8string_view(In, Idx);
    }

    template <size_t Size>
    static constexpr void FromString(std::u8string_view In, char8_t (&Out)[Size])
    {
        if (In.size() > Size)
        {
            throw std::out_of_range("String is too big");
        }

        const size_t BytesCopied = In.copy(Out, In.size());
        std::ranges::fill_n(Out + BytesCopied, Size - BytesCopied, u8'\0');
    }
}