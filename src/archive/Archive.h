#pragma once

#include "../mmio/MmioFile.h"
#include "../utils/Align.h"

#include "Header.h"
#include "StreamHeader.h"
#include "StreamRunlist.h"

namespace L4
{
    class Archive
    {
    public:
        template<class... ArgTs>
        Archive(ArgTs&&... Args) :
            File(std::forward<ArgTs>(Args...)),
            SectorSize(GetHeader().SectorSize),
            StreamCount(GetHeader().StreamCount),
            FirstRunlistOffset(Align(sizeof(Header) + StreamCount * sizeof(StreamHeader), SectorSize))
        {

        }

        // vvv Low level API

        const Header& GetHeader() const noexcept
        {
            return *File.Get<Header>();
        }

        const StreamHeader& GetStreamHeader(uint32_t Idx) const noexcept
        {
            return *File.Get<StreamHeader>(sizeof(Header) + Idx * sizeof(StreamHeader));
        }

        const StreamRunlist& GetStreamRunlist(uint32_t Idx) const noexcept
        {
            return *File.Get<StreamRunlist>(FirstRunlistOffset + Idx * sizeof(StreamRunlist));
        }

        const void* GetSector(uint32_t Idx) const noexcept
        {
            return File.Get((size_t)Idx * SectorSize);
        }

        // ^^^ Low level API

        // vvv High level API

        ArchiveVersion GetVersion() const noexcept
        {
            return GetHeader().Version;
        }

        uint32_t GetSectorSize() const noexcept
        {
            return SectorSize;
        }

        uint32_t GetStreamCount() const noexcept
        {
            return StreamCount;
        }

        std::pair<uint32_t, std::u8string_view> GetSourceVersion() const noexcept
        {
            auto& Header = GetHeader();
            return { Header.SourceVersionNumeric, GetSV(Header.SourceVersion) };
        }

        std::pair<uint32_t, std::u8string_view> GetAppVersion() const noexcept
        {
            auto& Header = GetHeader();
            return { Header.AppVersionNumeric, GetSV(Header.AppVersion) };
        }

        const Guid& GetSourceGuid() const noexcept
        {
            return GetHeader().SourceGuid;
        }

        const Guid& GetAppGuid() const noexcept
        {
            return GetHeader().AppGuid;
        }

        std::u8string_view GetSourceName() const noexcept
        {
            return GetSV(GetHeader().SourceName);
        }

        std::u8string_view GetAppName() const noexcept
        {
            return GetSV(GetHeader().AppName);
        }

        // ^^^ High level API

    protected:
        MmioFile File;

        const uint32_t SectorSize;
        const uint32_t StreamCount;
        const size_t FirstRunlistOffset;
    };

    /*class StreamIterator
    {
    public:
        constexpr StreamIterator() noexcept = default;

        constexpr explicit StreamIterator(Archive* Archive, size_t Idx = 0) noexcept : Archive(Archive), Idx(Idx)
        {

        }

        constexpr StreamIterator(Archive& Archive, size_t Idx = 0) noexcept : StreamIterator(&Archive, Idx)
        {

        }

        [[nodiscard]] constexpr StreamView operator*() const noexcept
        {
            return StreamView(Archive, Idx);
        }

        constexpr StreamIterator& operator++() noexcept
        {
            ++Idx;
            return *this;
        }

        constexpr StreamIterator operator++(int) noexcept
        {
            StreamIterator Tmp = *this;
            ++Idx;
            return Tmp;
        }

        constexpr StreamIterator& operator--() noexcept
        {
            --Idx;
            return *this;
        }

        constexpr StreamIterator operator--(int) noexcept
        {
            StreamIterator Tmp = *this;
            --Idx;
            return Tmp;
        }

        constexpr StreamIterator& operator+=(const ptrdiff_t Off) noexcept
        {
            Idx += Off;
            return *this;
        }

        [[nodiscard]] constexpr StreamIterator operator+(const ptrdiff_t Off) const noexcept
        {
            StreamIterator Tmp = *this;
            Tmp += Off;
            return Tmp;
        }

        constexpr StreamIterator& operator-=(const ptrdiff_t Off) noexcept
        {
            Idx -= Off;
            return *this;
        }

        [[nodiscard]] constexpr StreamIterator operator-(const ptrdiff_t Off) const noexcept
        {
            StreamIterator Tmp = *this;
            Tmp -= Off;
            return Tmp;
        }

        [[nodiscard]] constexpr ptrdiff_t operator-(const StreamIterator& Right) const noexcept
        {
            return Idx - Right.Idx;
        }

        [[nodiscard]] constexpr StreamView operator[](const ptrdiff_t Off) const noexcept
        {
            return StreamView(Archive, Idx + Off);
        }

        [[nodiscard]] constexpr bool operator==(const StreamIterator& Right) const noexcept
        {
            return Archive == Right.Archive && Idx == Right.Idx;
        }

        [[nodiscard]] constexpr std::partial_ordering operator<=>(const StreamIterator& Right) const noexcept
        {
            if (Archive != Right.Archive)
            {
                return std::partial_ordering::unordered;
            }
            return Idx <=> Right.Idx;
        }

        [[nodiscard]] StreamIterator begin() noexcept
        {
            return StreamIterator(Archive, 0);
        }

        [[nodiscard]] StreamIterator end() noexcept
        {
            return StreamIterator(Archive, Archive->GetHeader().StreamCount);
        }

    private:
        Archive* Archive;
        size_t Idx;
    };*/
}