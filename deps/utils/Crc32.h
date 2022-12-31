#pragma once

#include <array>
#include <span>
#include <string>

namespace L4
{
    namespace Detail
    {
        static consteval auto CreateCrcTable(uint32_t Polynomial)
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

        static constexpr auto CrcTable = CreateCrcTable(0xEDB88320);

        uint32_t Crc32LoopIntrinsics(uint32_t Crc, std::span<const std::byte> Buffer);

        template <class T, size_t Extent>
        static constexpr __forceinline uint32_t Crc32Loop(uint32_t Crc, std::span<const T, Extent> Buffer)
        {
            for (const T Byte : Buffer)
            {
                Crc = Detail::CrcTable[uint8_t(Crc & 0xFF) ^ uint8_t(Byte)] ^ (Crc >> 8);
            }
            return Crc;
        }

        template <class T, size_t Extent>
        static constexpr __forceinline uint32_t Crc32(std::span<const T, Extent> Buffer)
        {
            if (std::is_constant_evaluated())
            {
                return ~Crc32Loop(~0u, Buffer);
            }
            else
            {
                return ~Crc32LoopIntrinsics(~0u, std::as_bytes(Buffer));
            }
        }
    }

    static constexpr __forceinline uint32_t Crc32(std::span<const std::byte> Buffer)
    {
        return Detail::Crc32(Buffer);
    }

    template <class T, size_t Extent>
    requires(!std::is_same_v<T, const std::byte>)
    static constexpr __forceinline uint32_t Crc32(std::span<const T, Extent> Buffer)
    {
        if (std::is_constant_evaluated())
        {
            static_assert(sizeof(T) == 1, "Constexpr Crc32 requires T to be of size 1.");
            return Detail::Crc32(Buffer);
        }
        
        return Crc32(std::as_bytes(Buffer));
    }

    static constexpr __forceinline uint32_t Crc32(const std::string_view String)
    {
        return Crc32(std::span(String.data(), String.size()));
    }
}