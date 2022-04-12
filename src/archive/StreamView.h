#pragma once

#include "Archive.h"

#include <span>

namespace L4
{
    class StreamView
    {
    public:
        constexpr explicit StreamView(const Archive& Archive, uint32_t StreamIdx) : Archive(Archive), StreamIdx(StreamIdx)
        {

        }

        // vvv Low level API

        const StreamHeader& GetHeader() const noexcept
        {
            return Archive.GetStreamHeader(StreamIdx);
        }

        const StreamRunlist& GetRunlist() const noexcept
        {
            return Archive.GetStreamRunlist(StreamIdx);
        }

        // ^^^ Low level API

        // vvv High level API

        const Guid& GetGuid() const noexcept
        {
            return GetHeader().Guid;
        }

        uint32_t GetVersion() const noexcept
        {
            return GetHeader().Version;
        }

        uint32_t GetElementSize() const noexcept
        {
            return GetHeader().ElementSize;
        }

        std::u8string_view GetName() const noexcept
        {
            return GetSV(GetHeader().Name);
        }

        std::span<const std::byte, 192> GetContext() const noexcept
        {
            return GetHeader().Context;
        }

        // ^^^ High level API

    private:
        const Archive& Archive;
        uint32_t StreamIdx;
    };
}