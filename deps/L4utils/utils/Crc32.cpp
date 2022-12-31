#include "Crc32.h"

namespace L4::Detail
{
    // We can't use CPU intrinsics because they use CRC32-C, and we use zlib/IEEE 802.3
    // https://github.com/SixLabors/ImageSharp/blob/f4f689ce67ecbcc35cebddba5aacb603e6d1068a/src/ImageSharp/Formats/Png/Zlib/Crc32.cs#L18
    // https://chromium.googlesource.com/chromium/src/+/HEAD/third_party/zlib/crc32_simd.c#24
    __forceinline uint32_t Crc32LoopSse(uint32_t Crc, std::span<const std::byte> Buffer)
    {
        const uint32_t crc = Crc;
        auto buf = (const uint8_t*)Buffer.data();
        size_t len = Buffer.size() & ~0xF;
        auto LeftoverBuffer = Buffer.subspan(len);

        /*
         * Definitions of the bit-reflected domain constants k1,k2,k3, etc and
         * the CRC32+Barrett polynomials given at the end of the paper.
         */
        alignas(__m128i) static const uint64_t k1k2[] = { 0x0154442bd4, 0x01c6e41596 };
        alignas(__m128i) static const uint64_t k3k4[] = { 0x01751997d0, 0x00ccaa009e };
        alignas(__m128i) static const uint64_t k5k0[] = { 0x0163cd6124, 0x0000000000 };
        alignas(__m128i) static const uint64_t poly[] = { 0x01db710641, 0x01f7011641 };
        __m128i x0, x1, x2, x3, x4, x5, x6, x7, x8, y5, y6, y7, y8;
        /*
         * There's at least one block of 64.
         */
        x1 = _mm_loadu_si128((__m128i*)(buf + 0x00));
        x2 = _mm_loadu_si128((__m128i*)(buf + 0x10));
        x3 = _mm_loadu_si128((__m128i*)(buf + 0x20));
        x4 = _mm_loadu_si128((__m128i*)(buf + 0x30));
        x1 = _mm_xor_si128(x1, _mm_cvtsi32_si128(crc));
        x0 = _mm_load_si128((__m128i*)k1k2);
        buf += 64;
        len -= 64;
        /*
         * Parallel fold blocks of 64, if any.
         */
        while (len >= 64)
        {
            x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
            x6 = _mm_clmulepi64_si128(x2, x0, 0x00);
            x7 = _mm_clmulepi64_si128(x3, x0, 0x00);
            x8 = _mm_clmulepi64_si128(x4, x0, 0x00);
            x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
            x2 = _mm_clmulepi64_si128(x2, x0, 0x11);
            x3 = _mm_clmulepi64_si128(x3, x0, 0x11);
            x4 = _mm_clmulepi64_si128(x4, x0, 0x11);
            y5 = _mm_loadu_si128((__m128i*)(buf + 0x00));
            y6 = _mm_loadu_si128((__m128i*)(buf + 0x10));
            y7 = _mm_loadu_si128((__m128i*)(buf + 0x20));
            y8 = _mm_loadu_si128((__m128i*)(buf + 0x30));
            x1 = _mm_xor_si128(x1, x5);
            x2 = _mm_xor_si128(x2, x6);
            x3 = _mm_xor_si128(x3, x7);
            x4 = _mm_xor_si128(x4, x8);
            x1 = _mm_xor_si128(x1, y5);
            x2 = _mm_xor_si128(x2, y6);
            x3 = _mm_xor_si128(x3, y7);
            x4 = _mm_xor_si128(x4, y8);
            buf += 64;
            len -= 64;
        }
        /*
         * Fold into 128-bits.
         */
        x0 = _mm_load_si128((__m128i*)k3k4);
        x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
        x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
        x1 = _mm_xor_si128(x1, x2);
        x1 = _mm_xor_si128(x1, x5);
        x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
        x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
        x1 = _mm_xor_si128(x1, x3);
        x1 = _mm_xor_si128(x1, x5);
        x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
        x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
        x1 = _mm_xor_si128(x1, x4);
        x1 = _mm_xor_si128(x1, x5);
        /*
         * Single fold blocks of 16, if any.
         */
        while (len >= 16)
        {
            x2 = _mm_loadu_si128((__m128i*)buf);
            x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
            x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
            x1 = _mm_xor_si128(x1, x2);
            x1 = _mm_xor_si128(x1, x5);
            buf += 16;
            len -= 16;
        }
        /*
         * Fold 128-bits to 64-bits.
         */
        x2 = _mm_clmulepi64_si128(x1, x0, 0x10);
        x3 = _mm_setr_epi32(~0, 0, ~0, 0);
        x1 = _mm_srli_si128(x1, 8);
        x1 = _mm_xor_si128(x1, x2);
        x0 = _mm_loadl_epi64((__m128i*)k5k0);
        x2 = _mm_srli_si128(x1, 4);
        x1 = _mm_and_si128(x1, x3);
        x1 = _mm_clmulepi64_si128(x1, x0, 0x00);
        x1 = _mm_xor_si128(x1, x2);
        /*
         * Barret reduce to 32-bits.
         */
        x0 = _mm_load_si128((__m128i*)poly);
        x2 = _mm_and_si128(x1, x3);
        x2 = _mm_clmulepi64_si128(x2, x0, 0x10);
        x2 = _mm_and_si128(x2, x3);
        x2 = _mm_clmulepi64_si128(x2, x0, 0x00);
        x1 = _mm_xor_si128(x1, x2);
        /*
         * Return the crc32.
         */
        return Crc32Loop(_mm_extract_epi32(x1, 1), LeftoverBuffer);
    }

    uint32_t Crc32LoopIntrinsics(uint32_t Crc, std::span<const std::byte> Buffer)
    {
        if (Buffer.size() >= 64)
        {
            return Crc32LoopSse(Crc, Buffer);
        }
        else
        {
            return Crc32Loop(Crc, Buffer);
        }
    }
}