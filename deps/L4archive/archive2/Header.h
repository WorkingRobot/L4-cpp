#pragma once

#include "Consts.h"

#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace L4
{
    struct Header
    {
        uint32_t Magic;
        ArchiveVersion Version;
        uint32_t SectorSize;
        uint32_t StreamCount;
        uint32_t SourceVersionNumeric;
        uint32_t AppVersionNumeric;
        char8_t SourceId[32];
        char8_t AppId[32];
        char8_t SourceName[32];
        char8_t AppName[32];
        char8_t SourceVersion[32];
        char8_t AppVersion[32];
        char8_t Environment[16];
        std::byte Unknown[24];
    };

    static_assert(sizeof(Header) == 256);
    static_assert(std::has_unique_object_representations_v<Header>);

    template <size_t Size>
    std::u8string_view GetSV(const char8_t (&Buffer)[Size])
    {
        // strnlen_s version of std::char_traits<char8_t>::length()
        size_t Count = 0;
        const char8_t* Buf = Buffer;
        while (Count < Size && *Buf)
        {
            ++Count;
            ++Buf;
        }

        return std::u8string_view(Buffer, Count);
    }

    template <size_t Size>
    void SetSV(char8_t (&Buffer)[Size], const std::u8string_view Src)
    {
        if (Src.size() > Size)
        {
            throw std::length_error("Input string is too large");
        }
        auto CopyCount = Src.copy(Buffer, Src.size());
        std::fill_n(Buffer + CopyCount, Size - CopyCount, '\0');
    }
}