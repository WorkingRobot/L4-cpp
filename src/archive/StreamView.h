#pragma once

#include "Archive.h"

#include <span>

namespace L4
{
    template <bool Writable>
    class StreamViewBase
    {
        using ArchiveT = std::conditional_t<Writable, ArchiveWritable, const Archive>;

    public:
        constexpr explicit StreamViewBase(ArchiveT& Archive, uint32_t StreamIdx) :
            Archive(&Archive), StreamIdx(StreamIdx)
        {
        }

        // vvv Low level API

        const StreamHeader& GetHeader() const noexcept
        {
            return Archive->GetStreamHeader(StreamIdx);
        }

        StreamHeader& GetHeader() const noexcept requires(Writable)
        {
            return Archive->GetStreamHeader(StreamIdx);
        }

        const StreamRunlist& GetRunlist() const noexcept
        {
            return Archive->GetStreamRunlist(StreamIdx);
        }

        StreamRunlist& GetRunlist() const noexcept requires(Writable)
        {
            return Archive->GetStreamRunlist(StreamIdx);
        }

        uint32_t GetCapacityInSectors() const noexcept
        {
            auto& Runlist = GetRunlist();
            if (Runlist.RunCount == 0)
            {
                return 0;
            }
            auto& LastRun = Runlist.Runs[Runlist.RunCount - 1];
            return LastRun.StreamSectorOffset + LastRun.SectorCount;
        }

        size_t GetCapacity() const noexcept
        {
            return GetCapacityInSectors() * Archive->GetSectorSize();
        }

    private:
        static constexpr auto MinHeapEntries = [](const auto& A, const auto& B) {
            return std::greater<> {}(A.SectorCount, B.SectorCount);
        };

    public:
        void Reserve(size_t NewCapacity) requires(Writable)
        {
            NewCapacity *= GetElementSize();

            uint32_t RequestedCapacity = Align(NewCapacity, Archive->GetSectorSize()) / Archive->GetSectorSize();
            uint32_t CurrentCapacity = GetCapacityInSectors();

            if (CurrentCapacity >= RequestedCapacity)
            {
                return;
            }

            uint32_t MoreSectorsNeeded = RequestedCapacity - CurrentCapacity;

            auto& Freelist = Archive->GetFreelist();
            auto& Runlist = GetRunlist();

            // Extend current run and extend archive
            if (Runlist.RunCount != 0)
            {
                auto& LastRun = Runlist.Runs[Runlist.RunCount - 1];
                // If the last run for this stream touches the end of the archive, we can extend as far as we want
                if (LastRun.SectorOffset + LastRun.SectorCount == Freelist.TotalSectorCount)
                {
                    Freelist.TotalSectorCount += MoreSectorsNeeded;
                    Archive->ReserveSectors(Freelist.TotalSectorCount);
                    LastRun.SectorCount += MoreSectorsNeeded;
                    return;
                }
            }

            // Utilize freelist
            while (Freelist.EntryCount != 0)
            {
                std::pop_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
                auto& LastEntry = Freelist.Entries[Freelist.EntryCount - 1];
                auto& NewRun = Runlist.Runs[Runlist.RunCount++];
                if (LastEntry.SectorCount > MoreSectorsNeeded)
                {
                    NewRun = {
                        .StreamSectorOffset = CurrentCapacity,
                        .SectorOffset = LastEntry.SectorOffset,
                        .SectorCount = MoreSectorsNeeded
                    };

                    LastEntry.SectorCount -= MoreSectorsNeeded;
                    LastEntry.SectorOffset += MoreSectorsNeeded;

                    std::push_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
                    return;
                }
                else
                {
                    NewRun = {
                        .StreamSectorOffset = CurrentCapacity,
                        .SectorOffset = LastEntry.SectorOffset,
                        .SectorCount = LastEntry.SectorCount
                    };

                    MoreSectorsNeeded -= LastEntry.SectorCount;
                    CurrentCapacity += LastEntry.SectorCount;

                    --Freelist.EntryCount;
                }
            }

            // Create new run and extend archive
            auto& NewRun = Runlist.Runs[Runlist.RunCount++];
            NewRun = {
                .StreamSectorOffset = CurrentCapacity,
                .SectorOffset = Freelist.TotalSectorCount,
                .SectorCount = MoreSectorsNeeded
            };
            Freelist.TotalSectorCount += MoreSectorsNeeded;
            Archive->ReserveSectors(Freelist.TotalSectorCount);
        }

        void Resize(size_t NewSize) requires(Writable)
        {
            auto& Runlist = GetRunlist();
            if (NewSize <= Runlist.Size)
            {
                Runlist.Size = NewSize;
            }
            else
            {
                Reserve(NewSize);
                Runlist.Size = NewSize;
            }
        }

        void ShrinkToFit() requires(Writable)
        {
            auto& Freelist = Archive->GetFreelist();
            auto& Runlist = GetRunlist();

            uint32_t CurrentCapacity = GetCapacityInSectors();
            uint32_t TargetCapacity = Align(Runlist.Size * GetElementSize(), Archive->GetSectorSize()) / Archive->GetSectorSize();

            uint32_t SectorsToFree = CurrentCapacity - TargetCapacity;
            while (SectorsToFree != 0)
            {
                auto& Run = Runlist.Runs[Runlist.RunCount - 1];
                if (Run.SectorCount > SectorsToFree)
                {
                    Freelist.Entries[Freelist.EntryCount++] = {
                        .SectorCount = SectorsToFree,
                        .SectorOffset = Run.SectorOffset + Run.SectorCount - SectorsToFree
                    };

                    Run.SectorCount -= SectorsToFree;

                    std::push_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
                    return;
                }
                else
                {
                    Freelist.Entries[Freelist.EntryCount++] = {
                        .SectorCount = Run.SectorCount,
                        .SectorOffset = Run.SectorOffset
                    };

                    SectorsToFree -= Run.SectorCount;

                    --Runlist.RunCount;

                    std::push_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
                }
            }
        }

        void Read(std::span<std::byte> Dst, size_t Offset) const
        {
            auto& Runlist = GetRunlist();

            if (Offset + Dst.size() > Runlist.Size)
            {
                throw std::out_of_range("Read will go out of bounds");
            }

            // Sector offset within the stream
            uint32_t StreamSectorIdx = Offset / Archive->GetSectorSize();
            // Byte offset within the current sector
            uint32_t SectorOffset = Offset % Archive->GetSectorSize();
            auto RunItr = std::lower_bound(Runlist.Runs, Runlist.Runs + Runlist.RunCount, StreamSectorIdx, [](const StreamRun& Interval, uint32_t Idx) {
                return Interval.StreamSectorOffset + Interval.SectorCount <= Idx;
            });
            if (RunItr == Runlist.Runs + Runlist.RunCount)
            {
                throw std::out_of_range("Starting read from out of bounds");
            }

            // Index of the current sector within the current run
            uint32_t RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;

            while (true)
            {
                uint32_t ReadAmt = std::min<uint32_t>((RunItr->SectorCount - RunSectorIdx) * Archive->GetSectorSize() - SectorOffset, Dst.size());

                auto SrcPtr = reinterpret_cast<const std::byte*>(Archive->GetSector(RunItr->SectorOffset + RunSectorIdx)) + SectorOffset;
                std::copy_n(SrcPtr, ReadAmt, Dst.begin());

                Dst = Dst.subspan(ReadAmt);
                if (Dst.empty())
                {
                    return;
                }

                // Because we either finish a read to Dst or we read to the end of a whole run,
                // we can assume that we will begin at the start of a new run for all subsequent iterations
                SectorOffset = 0;
                RunSectorIdx = 0;
                ++RunItr;
                if (RunItr == Runlist.Runs + Runlist.RunCount)
                {
                    throw std::out_of_range("Continuing read from out of bounds");
                }
            }
        }

        void Write(std::span<const std::byte> Src, size_t Offset) requires(Writable)
        {
            auto& Runlist = GetRunlist();

            if (Offset + Src.size() > Runlist.Size)
            {
                throw std::out_of_range("Write will go out of bounds");
            }

            // Sector offset within the stream
            uint32_t StreamSectorIdx = Offset / Archive->GetSectorSize();
            // Byte offset within the current sector
            uint32_t SectorOffset = Offset % Archive->GetSectorSize();
            auto RunItr = std::lower_bound(Runlist.Runs, Runlist.Runs + Runlist.RunCount, StreamSectorIdx, [](const StreamRun& Interval, uint32_t Idx) {
                return Interval.StreamSectorOffset + Interval.SectorCount <= Idx;
            });
            if (RunItr == Runlist.Runs + Runlist.RunCount)
            {
                throw std::out_of_range("Starting write from out of bounds");
            }

            // Index of the current sector within the current run
            uint32_t RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;

            while (true)
            {
                uint32_t ReadAmt = std::min<uint32_t>((RunItr->SectorCount - RunSectorIdx) * Archive->GetSectorSize() - SectorOffset, Src.size());

                auto DstPtr = reinterpret_cast<std::byte*>(Archive->GetSector(RunItr->SectorOffset + RunSectorIdx)) + SectorOffset;
                std::copy_n(Src.begin(), ReadAmt, DstPtr);

                Src = Src.subspan(ReadAmt);
                if (Src.empty())
                {
                    return;
                }

                // Because we either finish a write from Src or we write to the end of a whole run,
                // we can assume that we will begin at the start of a new run for all subsequent iterations
                SectorOffset = 0;
                RunSectorIdx = 0;
                ++RunItr;
                if (RunItr == Runlist.Runs + Runlist.RunCount)
                {
                    throw std::out_of_range("Continuing write from out of bounds");
                }
            }
        }

        // ^^^ Low level API

        // vvv High level API

        std::u8string_view GetId() const noexcept
        {
            return GetSV(GetHeader().Id);
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

        std::span<const std::byte, 184> GetContext() const noexcept
        {
            return GetHeader().Context;
        }

        // ^^^ High level API

    private:
        ArchiveT* Archive;
        uint32_t StreamIdx;
    };

    using StreamView = StreamViewBase<false>;
    using StreamViewWritable = StreamViewBase<true>;

}