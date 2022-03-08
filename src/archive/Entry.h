#pragma once

#include "../utils/EnumFlags.h"

#include <variant>

namespace L4
{
    static constexpr uint32_t SectorSize = 0x1000;
    static constexpr uint32_t ClusterSize = 0x10000;
    static constexpr uint32_t InvalidIdx = 0x00000000;
    static constexpr uint16_t EntryMagic = 0x21E6;

    enum class EntryType : uint8_t
    {
        Resident,
        Super,
        NonResidentSector,
        NonResidentCluster,

        Unknown
    };

    enum class EntrySize : uint8_t
    {
        Size8,
        Size16,
        Size24,
        Size32,
        Size48,
        Size64,
        Reserved1,
        Reserved2,

        Unknown
    };

    enum class EntryFlags : uint8_t
    {
        None = 0x00
    };

    template<>
    struct IsEnumFlag<EntryFlags> : std::bool_constant<true>
    {
    };

    struct EntryBase
    {
        uint16_t Magic;
        uint8_t Category : 3;
        EntryType Type : 2;
        EntrySize Size : 3;
        uint8_t Id;

        bool operator==(const EntryBase&) const = default;
    };

    struct EntryResident : public EntryBase
    {
    };

    struct EntryNonResidentSector : public EntryBase
    {
        uint32_t SectorIdx;
    };

    struct EntryNonResidentCluster : public EntryBase
    {
        uint32_t ClusterIdx;
    };

    struct EntrySuper : public EntryBase
    {
        uint8_t SubEntryCount;
        uint8_t Reserved[3];
    };

    template<class T>
    struct EntryTraits;

    template<EntrySize Size>
    static constexpr uint8_t GetEntrySize()
    {
        return GetEntrySize(Size);
    }

    static constexpr uint8_t GetEntrySize(EntrySize Size)
    {
        switch (Size)
        {
            case EntrySize::Size8:
                return 8;
            case EntrySize::Size16:
                return 16;
            case EntrySize::Size24:
                return 24;
            case EntrySize::Size32:
                return 32;
            case EntrySize::Size48:
                return 48;
            case EntrySize::Size64:
                return 64;
            default:
                return 0;
        }
    }

    template<class T>
    static constexpr EntryType GetEntryType()
    {
        return std::is_base_of_v<EntryResident, T>           ? EntryType::Resident           :
               std::is_base_of_v<EntrySuper, T>              ? EntryType::Super              :
               std::is_base_of_v<EntryNonResidentSector, T>  ? EntryType::NonResidentSector  :
               std::is_base_of_v<EntryNonResidentCluster, T> ? EntryType::NonResidentCluster :
                                                               EntryType::Unknown;
    }

    template<class T>
    static constexpr EntryType GetEntryTypeStrict()
    {
        return std::is_same_v<EntryResident, T>           ? EntryType::Resident           :
               std::is_same_v<EntrySuper, T>              ? EntryType::Super              :
               std::is_same_v<EntryNonResidentSector, T>  ? EntryType::NonResidentSector  :
               std::is_same_v<EntryNonResidentCluster, T> ? EntryType::NonResidentCluster :
                                                            EntryType::Unknown;
    }

    template<bool Test, auto... Vals>
    struct TmpAssert : std::bool_constant<Test>
    {
        static_assert(Test);
    };

    template<bool Test, auto... Vals>
    constexpr bool TmpAssertV = TmpAssert<Test, Vals...>::value;

    template<class T>
    struct EntryTraitsBase
    {
        static_assert(std::is_base_of_v<EntryBase, T>, "T is not an Entry");
        static_assert(std::is_aggregate_v<T>, "Entry is not an aggregate");

        static constexpr EntryType Type = GetEntryType<T>();
        static_assert(Type != EntryType::Unknown, "Entry is of unknown type");

        static constexpr EntrySize Size =
            sizeof(T) <= 8  ? EntrySize::Size8  :
            sizeof(T) <= 16 ? EntrySize::Size16 :
            sizeof(T) <= 24 ? EntrySize::Size24 :
            sizeof(T) <= 32 ? EntrySize::Size32 :
            sizeof(T) <= 48 ? EntrySize::Size48 :
            sizeof(T) <= 64 ? EntrySize::Size64 :
                              EntrySize::Unknown;
        static constexpr uint8_t SizeBytes = GetEntrySize<Size>();
        static_assert(TmpAssertV<sizeof(T) == SizeBytes, sizeof(T), SizeBytes>, "Entry size doesn't equal requested size");
        static_assert(TmpAssertV<std::has_unique_object_representations_v<T>, sizeof(T), SizeBytes>, "Entry does not use all bits");

        static constexpr T Create()
        {
            return { EntryMagic, 0, Type, Size, EntryTraits<T>::Id };
        }
    };

    template<class T>
    struct EntryTraits
    {

    };

    class BadEntryAccess : public std::exception
    {
    public:
        BadEntryAccess() noexcept = default;

        [[nodiscard]] const char* what() const noexcept override
        {
            return "bad entry access";
        }
    };

    template<class T, std::enable_if_t<EntryTraits<std::decay_t<T>>::Id, bool> = true>
    static bool VerifyEntry(const EntryBase& Entry) noexcept
    {
        return Entry == EntryTraits<std::decay_t<T>>::Create();
    }

    template<class T, std::enable_if_t<GetEntryTypeStrict<T>() != EntryType::Unknown, bool> = true>
    static bool VerifyEntry(const EntryBase& Entry) noexcept
    {
        return Entry.Type == GetEntryTypeStrict<T>();
    }

    template<class T>
    static const T* GetEntry(const EntryBase* Entry) noexcept
    {
        if (Entry && VerifyEntry<T>(Entry))
        {
            return (const T*)Entry;
        }
        return nullptr;
    }

    template<class T>
    static const T& GetEntry(const EntryBase& Entry)
    {
        if (VerifyEntry<T>(Entry))
        {
            return (const T&)Entry;
        }
        throw BadEntryAccess{};
    }
}