#pragma once

#include <type_traits>

namespace L4 {
    template <typename Enum>
    struct IsEnumFlag : std::bool_constant<false> {
    };

    template <typename Enum>
    constexpr bool IsEnumFlagV = IsEnumFlag<Enum>::value;

    template <typename Enum>
    concept EnumFlag = IsEnumFlagV<Enum>;

    template <EnumFlag Enum>
    Enum operator|(Enum lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    }

    template <EnumFlag Enum>
    Enum operator&(Enum lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    }

    template <EnumFlag Enum>
    Enum operator^(Enum lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        return static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
    }

    template <EnumFlag Enum>
    Enum operator~(Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        return static_cast<Enum>(~static_cast<underlying>(rhs));
    }

    template <EnumFlag Enum>
    Enum& operator|=(Enum& lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        lhs = static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));

        return lhs;
    }

    template <EnumFlag Enum>
    Enum& operator&=(Enum& lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        lhs = static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));

        return lhs;
    }

    template <EnumFlag Enum>
    Enum& operator^=(Enum& lhs, Enum rhs)
    {
        using underlying = std::underlying_type_t<Enum>;

        lhs = static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));

        return lhs;
    }
}