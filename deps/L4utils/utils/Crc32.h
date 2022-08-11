#pragma once

#include <algorithm>
#include <array>
#include <span>
#include <string>

namespace L4
{
    namespace Detail
    {
        template <uint32_t Polynomial>
        static consteval auto CreateCrcTable()
        {
            std::array<uint32_t, 256> Table {};
            for (int Idx = 0; Idx < 256; ++Idx)
            {
                uint32_t Crc = Idx;
                for (int K = 0; K < 8; ++K)
                {
                    Crc = (Polynomial & (-(Crc & 1))) ^ (Crc >> 1);
                }
                Table[Idx] = Crc;
            }
            return Table;
        }

        static constexpr auto CrcTable = CreateCrcTable<0xEDB88320>();
    }

    static constexpr __forceinline uint32_t Crc32(std::span<const std::byte> Buffer)
    {
        uint32_t Crc = ~0u;
        for (const auto Byte : Buffer)
        {
            Crc = Detail::CrcTable[uint8_t(Crc & 0xFF) ^ uint8_t(Byte)] ^ (Crc >> 8);
        }
        return ~Crc;
    }

    template <class T, size_t Extent>
    requires(!std::is_same_v<T, const std::byte>)
    static constexpr __forceinline uint32_t Crc32(std::span<T, Extent> Buffer)
    {
        return Crc32(std::as_bytes(Buffer));
    }

    static constexpr __forceinline uint32_t Crc32(const std::string_view String)
    {
        return Crc32(std::span(String.data(), String.size()));
    }
}