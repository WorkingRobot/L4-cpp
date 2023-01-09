#include "ArchiveWritable.h"

#include "StreamWritable.h"
#include "utils/Align.h"

namespace L4::Archive
{
    ArchiveWritable::ArchiveWritable(const std::filesystem::path& Path) :
        File(Path),
        SectorSize(GetHeaderUnlocked().SectorSize),
        FreelistOffset(Align(sizeof(Header) + GetHeaderUnlocked().MaxStreamCount * sizeof(StreamHeader), SectorSize)),
        FileDataOffset(Align(FreelistOffset + sizeof(Freelist) + GetHeaderUnlocked().MaxStreamCount * sizeof(StreamRunlist), SectorSize))
    {
    }

    Header ArchiveWritable::GetHeader() const
    {
        std::shared_lock Lock(HeadersMtx);

        return GetHeaderUnlocked();
    }

    void ArchiveWritable::SetHeaderMeta(const Header& NewHeader)
    {
        std::lock_guard Lock(HeadersMtx);
        Header& OldHeader = GetHeaderUnlocked();

        OldHeader.PluginVersionNumeric = NewHeader.PluginVersionNumeric;
        OldHeader.AppVersionNumeric = NewHeader.AppVersionNumeric;
        std::ranges::copy(NewHeader.PluginId, OldHeader.PluginId);
        std::ranges::copy(NewHeader.AppId, OldHeader.AppId);
        std::ranges::copy(NewHeader.PluginName, OldHeader.PluginName);
        std::ranges::copy(NewHeader.AppName, OldHeader.AppName);
        std::ranges::copy(NewHeader.PluginVersion, OldHeader.PluginVersion);
        std::ranges::copy(NewHeader.AppVersion, OldHeader.AppVersion);
        std::ranges::copy(NewHeader.Environment, OldHeader.Environment);
    }

    StreamWritable ArchiveWritable::CreateStream(const StreamHeader& NewHeader)
    {
        std::lock_guard Lock(HeadersMtx);

        uint32_t NewIdx = GetFirstEmptyStreamIdxUnlocked();

        StreamWritable Ret(*this, NewIdx);
        Ret.SetHeaderUnlocked(NewHeader);

        GetHeaderUnlocked().StreamCount++;

        return Ret;
    }

    void ArchiveWritable::DeleteStream(uint32_t Idx)
    {
        std::scoped_lock Lock(HeadersMtx, RunlistsMtx);

        StreamWritable(*this, Idx).DeleteUnlocked();

        GetHeaderUnlocked().StreamCount--;
    }

    StreamWritable ArchiveWritable::GetStream(std::u8string_view Id)
    {
        std::shared_lock Lock(HeadersMtx);

        const auto& Header = GetHeaderUnlocked();

        for (uint32_t Idx = 0; Idx < Header.MaxStreamCount; ++Idx)
        {
            StreamWritable Stream(*this, Idx);
            if (ToString(Stream.GetHeaderUnlocked().Id) == Id)
            {
                return Stream;
            }
        }

        throw std::invalid_argument("No stream could be found under that id");
    }

    StreamWritable ArchiveWritable::GetOrCreateStream(const StreamHeader& Header)
    {
        std::unique_lock Lock(HeadersMtx);

        auto& ArHeader = GetHeaderUnlocked();

        uint32_t NewIdx = -1;
        for (uint32_t Idx = 0; Idx < ArHeader.MaxStreamCount; ++Idx)
        {
            StreamWritable Stream(*this, Idx);

            if (Stream.IsValidUnlocked())
            {
                if (std::ranges::equal(Stream.GetHeaderUnlocked().Id, Header.Id))
                {
                    return Stream;
                }
            }
            else if (NewIdx == -1)
            {
                NewIdx = Idx;
            }
        }

        if (NewIdx == -1)
        {
            throw std::out_of_range("No new streams can be created");
        }

        StreamWritable Ret(*this, NewIdx);
        Ret.SetHeaderUnlocked(Header);

        ArHeader.StreamCount++;

        return Ret;
    }

    const Header& ArchiveWritable::GetHeaderUnlocked() const noexcept
    {
        return *File.Get<Header>(0);
    }

    Header& ArchiveWritable::GetHeaderUnlocked() noexcept
    {
        return *File.Get<Header>(0);
    }

    Freelist& ArchiveWritable::GetFreelistUnlocked() noexcept
    {
        return *File.Get<Freelist>(FreelistOffset);
    }

    std::span<std::byte> ArchiveWritable::GetSectorsUnlocked(uint32_t SectorIdx, uint32_t SectorCount) noexcept
    {
        return std::span(File.Get(SectorIdx * SectorSize), SectorCount * SectorSize);
    }

    uint32_t ArchiveWritable::GetFirstEmptyStreamIdxUnlocked() const
    {
        const auto& Header = GetHeaderUnlocked();

        if (Header.StreamCount == Header.MaxStreamCount)
        {
            throw std::out_of_range("No new streams can be created");
        }

        for (uint32_t Idx = 0; Idx < Header.MaxStreamCount; ++Idx)
        {
            if (!StreamWritable(*(ArchiveWritable*)this, Idx).IsValidUnlocked())
            {
                return Idx;
            }
        }

        throw std::out_of_range("No new streams can be created");
    }
}