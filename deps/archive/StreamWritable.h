#pragma once

#include "ArchiveWritable.h"
#include "StreamHeader.h"
#include "StreamRunlist.h"

#include <span>

namespace L4::Archive
{
    class StreamWritable
    {
    public:
        StreamWritable(ArchiveWritable& Ar, uint32_t Idx);

        [[nodiscard]] bool IsValid() const;

        [[nodiscard]] StreamHeader GetHeader() const;

        [[nodiscard]] uint64_t GetCapacity() const;

        [[nodiscard]] uint64_t GetSize() const;

        void SetHeader(const StreamHeader& NewHeader);

        void ReserveCapacity(uint64_t NewCapacity);

        void ShrinkToFit();

        void Resize(uint64_t NewSize);

        void WriteBytes(uint64_t Offset, std::span<const std::byte> Src);

        void ReadBytes(uint64_t Offset, std::span<std::byte> Dst) const;

    private:
        friend class ArchiveWritable;

        template <class FuncT>
        void IterateRangeUnlocked(uint64_t Offset, uint64_t Size, FuncT Func) const;

        void ReserveCapacityUnlocked(uint64_t NewCapacity);

        void ShrinkToFitUnlocked();

        void ResizeUnlocked(uint64_t NewSize);

        void DeleteUnlocked();

        uint32_t GetCapacityInSectorsUnlocked() const noexcept;

        bool IsValidUnlocked() const noexcept;

        StreamHeader& GetHeaderUnlocked() noexcept;

        const StreamHeader& GetHeaderUnlocked() const noexcept;

        void SetHeaderUnlocked(const StreamHeader& NewHeader) noexcept;

        StreamRunlist& GetRunlistUnlocked() noexcept;

        const StreamRunlist& GetRunlistUnlocked() const noexcept;

        ArchiveWritable& Ar;
        const uint32_t Idx;
    };
}