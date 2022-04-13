#pragma once

#include "../mmio/MmioFile.h"
#include "../utils/Align.h"

#include "Freelist.h"
#include "Header.h"
#include "StreamHeader.h"
#include "StreamRunlist.h"

namespace L4
{
    template<bool Writable>
    class ArchiveBase
    {
    public:
        template<class... ArgTs>
        ArchiveBase(ArgTs&&... Args) :
            File(std::forward<ArgTs>(Args)...),
            SectorSize(GetHeader().SectorSize),
            StreamCount(GetHeader().StreamCount),
            FreelistOffset(Align(sizeof(Header) + StreamCount * sizeof(StreamHeader), SectorSize))
        {

        }

        // vvv Low level API

        const Header& GetHeader() const noexcept
        {
            return *File.Get<Header>();
        }
        
        Header& GetHeader() const noexcept requires(Writable)
        {
            return *File.Get<Header>();
        }

        const StreamHeader& GetStreamHeader(uint32_t Idx) const noexcept
        {
            return *File.Get<StreamHeader>(sizeof(Header) + Idx * sizeof(StreamHeader));
        }

        StreamHeader& GetStreamHeader(uint32_t Idx) const noexcept requires(Writable)
        {
            return *File.Get<StreamHeader>(sizeof(Header) + Idx * sizeof(StreamHeader));
        }

        const Freelist& GetFreelist() const noexcept
        {
            return *File.Get<Freelist>(FreelistOffset);
        }

        Freelist& GetFreelist() const noexcept requires(Writable)
        {
            return *File.Get<Freelist>(FreelistOffset);
        }

        const StreamRunlist& GetStreamRunlist(uint32_t Idx) const noexcept
        {
            return *File.Get<StreamRunlist>(FreelistOffset + sizeof(Freelist) + Idx * sizeof(StreamRunlist));
        }

        StreamRunlist& GetStreamRunlist(uint32_t Idx) const noexcept requires(Writable)
        {
            return *File.Get<StreamRunlist>(FreelistOffset + sizeof(Freelist) + Idx * sizeof(StreamRunlist));
        }

        const void* GetSector(uint32_t Idx) const noexcept
        {
            return File.Get((size_t)Idx * SectorSize);
        }

        void* GetSector(uint32_t Idx) const noexcept requires(Writable)
        {
            return File.Get((size_t)Idx * SectorSize);
        }

        void ReserveSectors(uint32_t SectorCount) requires(Writable)
        {
            File.Reserve(SectorCount * SectorSize);
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

    private:
        std::conditional_t<Writable, MmioFileWritable, MmioFile> File;

        const uint32_t SectorSize;
        const uint32_t StreamCount;
        const size_t FreelistOffset;
    };

    using Archive = ArchiveBase<false>;
    using ArchiveWritable = ArchiveBase<true>;
}