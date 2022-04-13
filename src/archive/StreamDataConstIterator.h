#pragma once

#include "Archive.h"

namespace L4
{
    template<class T = std::byte, class ArchiveT = const Archive>
    class StreamDataConstIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        constexpr StreamDataConstIterator() noexcept = default;

        constexpr explicit StreamDataConstIterator(ArchiveT& Archive, uint32_t StreamIdx, size_t Offset = 0) noexcept : Archive(&Archive), StreamIdx(StreamIdx)
        {
            Offset *= sizeof(T);

            auto& Header = Archive.GetStreamHeader(StreamIdx);
            auto& Runlist = Archive.GetStreamRunlist(StreamIdx);
            StreamSectorIdx = Offset / Archive.GetSectorSize();
            SectorOffset = Offset % Archive.GetSectorSize();
            auto RunItr = std::lower_bound(Runlist.Runs, Runlist.Runs + Runlist.RunCount, StreamSectorIdx, [](const StreamRun& Interval, uint32_t Idx)
            {
                return Interval.StreamSectorOffset + Interval.SectorCount <= Idx;
            });
            if (RunItr != Runlist.Runs + Runlist.RunCount)
            {
                StreamRunIdx = RunItr - Runlist.Runs;
                RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;
                FileSectorIdx = RunItr->SectorOffset + RunSectorIdx;
            }
            else
            {
                StreamRunIdx = -1;
                RunSectorIdx = -1;
                FileSectorIdx = -1;
            }
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return *GetPtr();
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return GetPtr();
        }

        constexpr StreamDataConstIterator& operator++() noexcept
        {
            Increment();
            return *this;
        }

        constexpr StreamDataConstIterator operator++(int) noexcept
        {
            StreamDataConstIterator Tmp = *this;
            Increment();
            return Tmp;
        }

        constexpr StreamDataConstIterator& operator--() noexcept
        {
            Decrement();
            return *this;
        }

        constexpr StreamDataConstIterator operator--(int) noexcept
        {
            StreamDataConstIterator Tmp = *this;
            Decrement();
            return Tmp;
        }

        constexpr StreamDataConstIterator& operator+=(const ptrdiff_t Offset) noexcept
        {
            Seek(Offset);
            return *this;
        }

        [[nodiscard]] constexpr StreamDataConstIterator operator+(const ptrdiff_t Offset) const noexcept
        {
            StreamDataConstIterator Tmp = *this;
            Tmp += Offset;
            return Tmp;
        }

        constexpr StreamDataConstIterator& operator-=(const ptrdiff_t Offset) noexcept
        {
            Seek(-Offset);
            return *this;
        }

        [[nodiscard]] constexpr StreamDataConstIterator operator-(const ptrdiff_t Offset) const noexcept
        {
            StreamDataConstIterator Tmp = *this;
            Tmp -= Offset;
            return Tmp;
        }

        [[nodiscard]] constexpr ptrdiff_t operator-(const StreamDataConstIterator& _Right) const noexcept
        {
            return GetPosition() - _Right.GetPosition();
        }

        [[nodiscard]] constexpr reference operator[](const ptrdiff_t Offset) const noexcept
        {
            return *(*this + Offset);
        }

        [[nodiscard]] constexpr bool operator==(const StreamDataConstIterator& _Right) const noexcept
        {
            return GetPosition() == _Right.GetPosition();
        }

        [[nodiscard]] constexpr std::strong_ordering operator<=>(const StreamDataConstIterator& _Right) const noexcept
        {
            return GetPosition() <=> _Right.GetPosition();
        }

    private:
        constexpr bool Increment(size_t Count = 1) noexcept
        {
            Count *= sizeof(T);

            SectorOffset += Count;

            // Within sector
            if (SectorOffset < Archive->GetSectorSize())
            {
                return true;
            }

            // Within run
            StreamSectorIdx += SectorOffset / Archive->GetSectorSize();
            SectorOffset = SectorOffset % Archive->GetSectorSize();
            auto& Runlist = Archive->GetStreamRunlist(StreamIdx);
            if (auto& Run = Runlist.Runs[StreamRunIdx]; Run.StreamSectorOffset + Run.SectorOffset < StreamSectorIdx)
            {
                RunSectorIdx = StreamSectorIdx - Run.StreamSectorOffset;
                FileSectorIdx = Run.SectorOffset + RunSectorIdx;
                return true;
            }

            // Within file
            auto RunItr = std::find_if(Runlist.Runs + StreamRunIdx, Runlist.Runs + Runlist.RunCount, [Idx = StreamSectorIdx](const StreamRun& Interval)
            {
                return Interval.StreamSectorOffset + Interval.SectorCount > Idx;
            });
            if (RunItr != Runlist.Runs + Runlist.RunCount)
            {
                StreamRunIdx = RunItr - Runlist.Runs;
                RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;
                FileSectorIdx = RunItr->SectorOffset + RunSectorIdx;
                return true;
            }

            return false;
        }

        constexpr bool Decrement(size_t Count = 1) noexcept
        {
            Count *= sizeof(T);

            // Within sector
            if (Count <= SectorOffset)
            {
                SectorOffset -= Count;
                return true;
            }

            size_t AbsoluteOffset = (size_t)StreamSectorIdx * Archive->GetSectorSize() + SectorOffset;
            if (AbsoluteOffset < Count)
            {
                return false;
            }
            AbsoluteOffset -= Count;

            // Within run
            StreamSectorIdx = AbsoluteOffset / Archive->GetSectorSize();
            SectorOffset = AbsoluteOffset % Archive->GetSectorSize();
            auto& Runlist = Archive->GetStreamRunlist(StreamIdx);
            if (auto& Run = Runlist.Runs[StreamRunIdx]; StreamSectorIdx >= Run.StreamSectorOffset)
            {
                RunSectorIdx = StreamSectorIdx - Run.StreamSectorOffset;
                FileSectorIdx = Run.SectorOffset + RunSectorIdx;
                return true;
            }

            // Within file
            auto RunItr = &*std::find_if(std::make_reverse_iterator(Runlist.Runs + StreamRunIdx), std::make_reverse_iterator(Runlist.Runs), [Idx = StreamSectorIdx](const StreamRun& Interval)
            {
                return Interval.StreamSectorOffset < Idx;
            });

            // Expected: RunItr != std::make_reverse_iterator(Runlist.Runs)
            // We already checked it can't be behind 0

            StreamRunIdx = RunItr - Runlist.Runs;
            RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;
            FileSectorIdx = RunItr->SectorOffset + RunSectorIdx;
            return true;
        }

        constexpr bool Seek(ptrdiff_t Offset) noexcept
        {
            return Offset < 0 ? Decrement(-Offset) : Increment(Offset);
        }

        [[nodiscard]] constexpr const T* GetPtr() const noexcept
        {
            return reinterpret_cast<const T*>(static_cast<const char*>(Archive->GetSector(FileSectorIdx)) + SectorOffset);
        }

        [[nodiscard]] constexpr size_t GetPosition() const noexcept
        {
            return ((size_t)StreamSectorIdx * Archive->GetSectorSize() + SectorOffset) / sizeof(T);
        }

    protected:
        ArchiveT* Archive;

        // Index of stream being read from
        uint32_t StreamIdx;
        // Sector in file
        uint32_t FileSectorIdx;
        // Offset within sector
        uint32_t SectorOffset;
        // Sector in stream
        uint32_t StreamSectorIdx;
        // Sector in current run
        uint32_t RunSectorIdx;
        // Index of run in stream's runlist
        uint32_t StreamRunIdx;
    };
}