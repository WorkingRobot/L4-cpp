#pragma once

#include "Freelist.h"
#include "Header.h"
#include "mmio/MmioFile.h"

#include <span>

namespace L4::Archive
{
    class Stream;

    class Archive
    {
    public:
        Archive(const std::filesystem::path& Path);

        [[nodiscard]] const Header& GetHeader() const noexcept;

        [[nodiscard]] Stream GetStream(std::u8string_view Id) const;

    private:
        const Freelist& GetFreelist() const noexcept;

        std::span<const std::byte> GetSectors(uint32_t SectorIdx, uint32_t SectorCount = 1) const noexcept;

        friend class Stream;

        const Mmio::MmioFile File;

        const uint32_t SectorSize;
        const size_t FreelistOffset;
        const size_t FileDataOffset;
    };
}