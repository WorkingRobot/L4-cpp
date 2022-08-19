#pragma once

#include <rapidjson/document.h>

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace L4::Web::Json
{
    enum class Encoding : uint8_t
    {
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE
    };

    namespace Detail
    {
        template <Encoding T>
        struct JsonTraits
        {
        };

        template <>
        struct JsonTraits<Encoding::UTF8>
        {
            using CharT = char8_t;
            using EncodingT = rapidjson::UTF8<CharT>;
        };

        template <>
        struct JsonTraits<Encoding::UTF16LE>
        {
            using CharT = char16_t;
            using EncodingT = rapidjson::UTF16LE<CharT>;
        };

        template <>
        struct JsonTraits<Encoding::UTF16BE>
        {
            using CharT = char16_t;
            using EncodingT = rapidjson::UTF16BE<CharT>;
        };

        template <>
        struct JsonTraits<Encoding::UTF32LE>
        {
            using CharT = char32_t;
            using EncodingT = rapidjson::UTF32LE<CharT>;
        };

        template <>
        struct JsonTraits<Encoding::UTF32BE>
        {
            using CharT = char32_t;
            using EncodingT = rapidjson::UTF32BE<CharT>;
        };

        template <Encoding Enc>
        using JsonEncodingT = typename JsonTraits<Enc>::EncodingT;

        template <Encoding Enc>
        using JsonCharT = typename JsonTraits<Enc>::CharT;

        template<class Encoding>
        struct JsonEncoding
        {
        };

        template <>
        struct JsonEncoding<JsonEncodingT<Encoding::UTF8>>
        {
            static constexpr Encoding Encoding = Encoding::UTF8;
        };

        template <>
        struct JsonEncoding<JsonEncodingT<Encoding::UTF16LE>>
        {
            static constexpr Encoding Encoding = Encoding::UTF16LE;
        };

        template <>
        struct JsonEncoding<JsonEncodingT<Encoding::UTF16BE>>
        {
            static constexpr Encoding Encoding = Encoding::UTF16BE;
        };

        template <>
        struct JsonEncoding<JsonEncodingT<Encoding::UTF32LE>>
        {
            static constexpr Encoding Encoding = Encoding::UTF32LE;
        };

        template <>
        struct JsonEncoding<JsonEncodingT<Encoding::UTF32BE>>
        {
            static constexpr Encoding Encoding = Encoding::UTF32BE;
        };

        template <class EncodingT>
        static constexpr Encoding JsonEncodingEnumT = JsonEncoding<EncodingT>::Encoding;
    }

    template <Encoding Enc = Encoding::UTF8>
    using DocumentBase = rapidjson::GenericDocument<Detail::JsonEncodingT<Enc>>;

    template <Encoding Enc = Encoding::UTF8>
    using ValueBase = rapidjson::GenericValue<Detail::JsonEncodingT<Enc>>;

    using Document = DocumentBase<>;
    using Value = ValueBase<>;

    template <Encoding Enc = Encoding::UTF8>
    static DocumentBase<Enc> Parse(const std::basic_string_view<Detail::JsonCharT<Enc>>& Data)
    {
        DocumentBase<Enc> Json;
        Json.Parse(Data.data(), Data.size());
        return Json;
    }

    template <typename T>
    struct Parser
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, T& Obj) const
        {
            return Obj.Parse(Json);
        }
    };

    template <class CharT>
    struct Parser<std::basic_string<CharT>>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, std::basic_string<CharT>& Obj) const
        {
            static_assert(std::is_same_v<CharT, typename Encoding::Ch>, "String character types must match");

            if (Json.IsString())
            {
                Obj.assign(Json.GetString(), Json.GetStringLength());
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<bool>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, bool& Obj) const
        {
            if (Json.IsBool())
            {
                Obj = Json.GetBool();
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<int32_t>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, int32_t& Obj) const
        {
            if (Json.IsInt())
            {
                Obj = Json.GetInt();
                return true;
            }
            return false;
        }
    };

    template<>
    struct Parser<uint32_t>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, uint32_t& Obj) const
        {
            if (Json.IsUint())
            {
                Obj = Json.GetUint();
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<int64_t>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, int64_t& Obj) const
        {
            if (Json.IsInt64())
            {
                Obj = Json.GetInt64();
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<uint64_t>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, uint64_t& Obj) const
        {
            if (Json.IsUInt64())
            {
                Obj = Json.GetUInt64();
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<float>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, float& Obj) const
        {
            if (Json.IsFloat())
            {
                Obj = Json.GetFloat();
                return true;
            }
            return false;
        }
    };

    template <>
    struct Parser<double>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, double& Obj) const
        {
            if (Json.IsDouble())
            {
                Obj = Json.GetDouble();
                return true;
            }
            return false;
        }
    };

    template <typename Encoding>
    struct Parser<rapidjson::GenericDocument<Encoding>>
    {
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, rapidjson::GenericDocument<Encoding>& Obj) const
        {
            Obj.CopyFrom(Json, Obj.GetAllocator(), false);
            return true;
        }
    };

    template <typename T>
    struct Parser<std::optional<T>>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, std::optional<T>& Obj) const
        {
            return Parser<T> {}(Json, Obj.emplace());
        }
    };

    template <typename T>
    struct Parser<std::vector<T>>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, std::vector<T>& Obj) const
        {
            if (Json.IsArray())
            {
                Obj.clear();
                Obj.reserve(Json.Size());
                for (auto& Value : Json.GetArray())
                {
                    if (!Parser<T> {}(Value, Obj.emplace_back()))
                    {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
    };

    template <class CharT, typename T>
    struct Parser<std::unordered_map<std::basic_string<CharT>, T>>
    {
        template <typename Encoding>
        bool operator()(const rapidjson::GenericValue<Encoding>& Json, std::unordered_map<std::basic_string<CharT>, T>& Obj) const
        {
            if (Json.IsObject())
            {
                Obj.clear();
                Obj.reserve(Json.MemberCount());
                for (auto& Value : Json.GetObject())
                {
                    auto Item = Obj.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(Value.name.GetString(), Value.name.GetStringLength()),
                        std::forward_as_tuple());
                    if (!Item.second)
                    {
                        return false;
                    }
                    if (!Parser<T> {}(Value.value, Item.first->second))
                    {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
    };
}