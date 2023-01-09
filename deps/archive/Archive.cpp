#include "Archive.h"

#include "Stream.h"
#include "utils/Align.h"

namespace L4::Archive
{
    Archive::Archive(const std::filesystem::path& Path) :
        File(Path),
        SectorSize(GetHeader().SectorSize),
        FreelistOffset(Align(sizeof(Header) + GetHeader().MaxStreamCount * sizeof(StreamHeader), SectorSize)),
        FileDataOffset(Align(FreelistOffset + sizeof(Freelist) + GetHeader().MaxStreamCount * sizeof(StreamRunlist), SectorSize))
    {
    }

    const Header& Archive::GetHeader() const noexcept
    {
        return *File.Get<Header>(0);
    }

    Stream Archive::GetStream(std::u8string_view Id) const
    {
        const auto& Header = GetHeader();

        for (uint32_t Idx = 0; Idx < Header.MaxStreamCount; ++Idx)
        {
            Stream Stream(*this, Idx);
            if (ToString(Stream.GetHeader().Id) == Id)
            {
                return Stream;
            }
        }

        throw std::invalid_argument("No stream could be found under that id");
    }

    const Freelist& Archive::GetFreelist() const noexcept
    {
        return *File.Get<Freelist>(FreelistOffset);
    }

    std::span<const std::byte> Archive::GetSectors(uint32_t SectorIdx, uint32_t SectorCount) const noexcept
    {
        return std::span(File.Get(SectorIdx * SectorSize), SectorCount * SectorSize);
    }
}