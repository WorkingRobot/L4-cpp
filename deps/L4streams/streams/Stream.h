#pragma once

#include <chrono>
#include <deque>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>

namespace L4
{
    template <class T>
    struct Serializer;

    class Stream
    {
    public:
        enum class SeekPosition : int8_t
        {
            Beg,
            Cur,
            End
        };

        static constexpr SeekPosition Beg = SeekPosition::Beg;
        static constexpr SeekPosition Cur = SeekPosition::Cur;
        static constexpr SeekPosition End = SeekPosition::End;

        virtual void WriteBytes(const std::byte* Src, size_t ByteCount) = 0;
        virtual void ReadBytes(std::byte* Dst, size_t ByteCount) = 0;
        virtual void Seek(ptrdiff_t Position, SeekPosition SeekFrom) = 0;
        virtual size_t Tell() const = 0;
        virtual size_t Size() const = 0;

        // Write ops

        template <class T>
        inline void Write(const T& Val)
        {
            Serializer<T>::Serialize(*this, Val);
        }

        template <class T>
        inline Stream& operator<<(const T& Val)
        {
            Serializer<T>::Serialize(*this, Val);
            return *this;
        }

        template <class T>
        [[nodiscard]] inline T Read()
        {
            T Val {};
            Serializer<T>::Deserialize(*this, Val);
            return Val;
        }

        template <class T>
        inline Stream& operator>>(T& Val)
        {
            Serializer<T>::Deserialize(*this, Val);
            return *this;
        }
    };

    template <class StreamT, class StringT = std::string, class... ArgTs>
    static StringT DumpStreamData(ArgTs&&... Args)
    {
        StreamT Stream(std::forward<ArgTs>(Args)...);
        StringT DataString {};
        DataString.resize(Stream.Size() / sizeof(typename StringT::value_type));
        Stream.ReadBytes(reinterpret_cast<std::byte*>(DataString.data()), DataString.size() * sizeof(typename StringT::value_type));
        return DataString;
    }

    template <class T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
    struct Serializer<T>
    {
        static void Serialize(Stream& Stream, const T Val)
        {
            Stream.WriteBytes(reinterpret_cast<const std::byte*>(&Val), sizeof(Val));
        }

        static void Deserialize(Stream& Stream, T& Val)
        {
            Stream.ReadBytes(reinterpret_cast<std::byte*>(&Val), sizeof(Val));
        }
    };

    template <class T, size_t Extent>
    struct Serializer<std::span<T, Extent>>
    {
        static void Serialize(Stream& Stream, const std::span<T, Extent> Val)
        {
            Stream << Val.size();
            Stream.WriteBytes(reinterpret_cast<const std::byte*>(Val.data()), Val.size() * sizeof(T));
        }

        static void Deserialize(Stream& Stream, const std::span<T, Extent> Val)
        {
            auto Size = Stream.Read<size_t>();
            if (Size != Val.size())
            {
                throw std::invalid_argument("Span sizes do not match");
            }
            Stream.ReadBytes(reinterpret_cast<std::byte*>(Val.data()), Val.size() * sizeof(T));
        }
    };

    template <class T, class Traits>
    struct Serializer<std::basic_string_view<T, Traits>>
    {
        static void Serialize(Stream& Stream, const std::basic_string_view<T, Traits> Val)
        {
            Stream << Val.size();
            Stream.WriteBytes(reinterpret_cast<const std::byte*>(Val.data()), Val.size() * sizeof(T));
        }

        static void Deserialize(Stream& Stream, const std::basic_string_view<T, Traits> Val)
        {
            auto Size = Stream.Read<size_t>();
            if (Size != Val.size())
            {
                throw std::invalid_argument("String view do not match");
            }
            Stream.ReadBytes(reinterpret_cast<std::byte*>(Val.data()), Val.size() * sizeof(T));
        }
    };

    template <class T, class Traits, class Allocator>
    struct Serializer<std::basic_string<T, Traits, Allocator>>
    {
        static void Serialize(Stream& Stream, const std::basic_string<T, Traits, Allocator>& Val)
        {
            Stream << Val.size();
            Stream.WriteBytes(reinterpret_cast<const std::byte*>(Val.data()), Val.size() * sizeof(T));
        }

        static void Deserialize(Stream& Stream, std::basic_string<T, Traits, Allocator>& Val)
        {
            auto Size = Stream.Read<size_t>();
            Val.resize(Size);
            Stream.ReadBytes(reinterpret_cast<std::byte*>(Val.data()), Val.size() * sizeof(T));
        }
    };

    template <>
    struct Serializer<std::filesystem::path>
    {
        static void Serialize(Stream& Stream, const std::filesystem::path& Val)
        {
            Stream << Val.native();
        }

        static void Deserialize(Stream& Stream, std::filesystem::path& Val)
        {
            Val = Stream.Read<std::filesystem::path::string_type>();
        }
    };

    template <class T, size_t Size>
    struct Serializer<T (&)[Size]>
    {
        static void Serialize(Stream& Stream, const T (&Val)[Size])
        {
            for (size_t Idx = 0; Idx < Size; ++Idx)
            {
                Stream << Val[Idx];
            }
        }

        static void Deserialize(Stream& Stream, T (&Val)[Size])
        {
            for (size_t Idx = 0; Idx < Size; ++Idx)
            {
                Stream >> Val[Idx];
            }
        }
    };

    template <class T, class Allocator>
    struct Serializer<std::vector<T, Allocator>>
    {
        static void Serialize(Stream& Stream, const std::vector<T, Allocator>& Val)
        {
            Stream << Val.size();
            for (auto& Item : Val)
            {
                Stream << Item;
            }
        }

        static void Deserialize(Stream& Stream, std::vector<T, Allocator>& Val)
        {
            auto Size = Stream.Read<size_t>();
            Val.clear();
            Val.reserve(Size);
            for (size_t Idx = 0; Idx < Size; ++Idx)
            {
                Stream >> Val.emplace_back();
            }
        }
    };

    template <class T, class Allocator>
    struct Serializer<std::deque<T, Allocator>>
    {
        static void Serialize(Stream& Stream, const std::deque<T, Allocator>& Val)
        {
            Stream << Val.size();
            for (auto& Item : Val)
            {
                Stream << Item;
            }
        }

        static void Deserialize(Stream& Stream, std::deque<T, Allocator>& Val)
        {
            auto Size = Stream.Read<size_t>();
            Val.clear();
            for (size_t Idx = 0; Idx < Size; ++Idx)
            {
                Stream >> Val.emplace_back();
            }
        }
    };

    template <class K, class V>
    struct Serializer<std::pair<K, V>>
    {
        static void Serialize(Stream& Stream, const std::pair<K, V>& Val)
        {
            Stream << Val.first;
            Stream << Val.second;
        }

        static void Deserialize(Stream& Stream, std::pair<K, V>& Val)
        {
            Stream >> Val.first;
            Stream >> Val.second;
        }
    };

    template <class K, class V, class Hash, class KeyEqual, class Allocator>
    struct Serializer<std::unordered_map<K, V, Hash, KeyEqual, Allocator>>
    {
        static void Serialize(Stream& Stream, const std::unordered_map<K, V, Hash, KeyEqual, Allocator>& Val)
        {
            Stream << Val.size();
            for (auto& Item : Val)
            {
                Stream << Item;
            }
        }

        static void Deserialize(Stream& Stream, std::unordered_map<K, V, Hash, KeyEqual, Allocator>& Val)
        {
            auto Size = Stream.Read<size_t>();
            Val.clear();
            Val.reserve(Size);
            for (size_t Idx = 0; Idx < Size; ++Idx)
            {
                Val.emplace(Stream.Read<std::pair<K, V>>());
            }
        }
    };

    template <class Clock, class Duration>
    struct Serializer<std::chrono::time_point<Clock, Duration>>
    {
        static void Serialize(Stream& Stream, const std::chrono::time_point<Clock, Duration>& Val)
        {
            Stream << Val.time_since_epoch();
        }

        static void Deserialize(Stream& Stream, std::chrono::time_point<Clock, Duration>& Val)
        {
            Val = decltype(Val)(Stream.Read<Duration>());
        }
    };

    template <class Rep, class Period>
    struct Serializer<std::chrono::duration<Rep, Period>>
    {
        static void Serialize(Stream& Stream, const std::chrono::duration<Rep, Period>& Val)
        {
            Stream << Val.count();
        }

        static void Deserialize(Stream& Stream, std::chrono::duration<Rep, Period>& Val)
        {
            Val = decltype(Val)(Stream.Read<Rep>());
        }
    };
}