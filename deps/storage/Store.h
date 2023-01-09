#pragma once

#include "streams/MemoryStream.h"
#include "streams/BufferStream.h"

#include <filesystem>
#include <span>
#include <string_view>

namespace L4::Storage
{
    class Store
    {
    public:
        Store(const std::filesystem::path& Path);

        ~Store();

        void SetRaw(const std::string_view Key, std::vector<std::byte>&& Data);

        template <class T>
        void Set(const std::string_view Key, const T& Data)
        {
            MemoryStream Stream;
            Stream << Data;
            SetRaw(Key, std::move(Stream.GetBuffer()));
        }

        const std::vector<std::byte>& GetRaw(const std::string_view Key) const;

        template<class T>
        T Get(const std::string_view Key) const
        {
            const auto& Value = GetRaw(Key);
            BufferStream Stream(std::span((std::byte*)Value.data(), Value.size()));
            return Stream.Read<T>();
        }

    private:
        struct Impl;
        std::unique_ptr<Impl> PImpl;
    };
}