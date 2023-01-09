#pragma once

#include "Freelist.h"
#include "Header.h"
#include "mmio/MmioFile.h"
#include "StreamHeader.h"

#include <shared_mutex>
#include <span>

namespace L4::Archive
{
    class StreamWritable;

    class ArchiveWritable
    {
    public:
        ArchiveWritable(const std::filesystem::path& Path);

        [[nodiscard]] Header GetHeader() const;

        void SetHeaderMeta(const Header& NewHeader);

        [[nodiscard]] StreamWritable CreateStream(const StreamHeader& NewHeader);

        void DeleteStream(uint32_t Idx);

        [[nodiscard]] StreamWritable GetStream(std::u8string_view Id);

        [[nodiscard]] StreamWritable GetOrCreateStream(const StreamHeader& Header);

    private:
        friend class StreamWritable;
        
        const Header& GetHeaderUnlocked() const noexcept;

        Header& GetHeaderUnlocked() noexcept;

        Freelist& GetFreelistUnlocked() noexcept;

        std::span<std::byte> GetSectorsUnlocked(uint32_t SectorIdx, uint32_t SectorCount = 1) noexcept;

        uint32_t GetFirstEmptyStreamIdxUnlocked() const;

        Mmio::MmioFileWritable File;

        const uint32_t SectorSize;
        const size_t FreelistOffset;
        const size_t FileDataOffset;

        mutable std::shared_mutex HeadersMtx;
        mutable std::shared_mutex RunlistsMtx;
    };
}