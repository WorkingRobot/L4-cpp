#pragma once

#include <string>
#include <array>

namespace L4
{
    namespace Detail
    {

        // https://stackoverflow.com/a/28801005
        template <uint32_t c, int k = 8>
        struct f : f<(c >> 1) ^ (0xEDB88320 & (-(c & 1))), k - 1> {};
        template <uint32_t c> struct f<c, 0> { enum { value = c }; };

#define A(c,x) B(c,x) B(c,x + 128)
#define B(c,x) C(c,x) C(c,x +  64)
#define C(c,x) D(c,x) D(c,x +  32)
#define D(c,x) E(c,x) E(c,x +  16)
#define E(c,x) F(c,x) F(c,x +   8)
#define F(c,x) G(c,x) G(c,x +   4)
#define G(c,x) H(c,x) H(c,x +   2)
#define H(c,x) I(c,x) I(c,x +   1)
#define I(c,x) c<x>::value,

        static constexpr uint32_t crc_table[] = { A(f, 0) };

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I

        static constexpr uint32_t crc32_impl(const uint8_t* p, size_t len, uint32_t crc)
        {
            return len ?
                crc32_impl(p + 1, len - 1, (crc >> 8) ^ crc_table[(uint8_t)(crc & 0xFF) ^ *p])
                : crc;
        }

        static constexpr uint32_t crc32_large(const uint8_t* p, size_t len, uint32_t crc)
        {
            while (len--)
            {
                crc = (crc >> 8) ^ crc_table[(uint8_t)(crc & 0xFF) ^ *(p++)];
            }
            return crc;
        }
    }

    static constexpr __forceinline uint32_t Crc32Large(const char* str, size_t size)
    {
        return ~Detail::crc32_large((uint8_t*)str, size, ~0);
    }

    static constexpr __forceinline uint32_t Crc32(const char* str, size_t size)
    {
        return ~Detail::crc32_impl((uint8_t*)str, size, ~0);
    }

    static __forceinline uint32_t Crc32(const std::string& str)
    {
        return Crc32(str.c_str(), str.size());
    }

    template<size_t size>
    static constexpr __forceinline uint32_t Crc32(const char(&str)[size])
    {
        return Crc32(str, size - 1);
    }
}