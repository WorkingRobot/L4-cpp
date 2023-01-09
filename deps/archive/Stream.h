#pragma once

#include "Archive.h"
#include "StreamHeader.h"
#include "StreamRunlist.h"

#include <span>

namespace L4::Archive
{
    class Stream
    {
    public:
        Stream(const Archive& Ar, uint32_t Idx);

        [[nodiscard]] bool IsValid() const noexcept;

        [[nodiscard]] const StreamHeader& GetHeader() const noexcept;

        [[nodiscard]] uint64_t GetCapacity() const noexcept;

        [[nodiscard]] uint64_t GetSize() const noexcept;

        void ReadBytes(uint64_t Offset, std::span<std::byte> Dst) const;

    private:
        template <class FuncT>
        void IterateRangeInternal(uint64_t Offset, uint64_t Size, FuncT Func) const;

        const StreamRunlist& GetRunlist() const noexcept;

        uint32_t GetCapacityInSectors() const noexcept;

        const Archive& Ar;
        const uint32_t Idx;
    };
}