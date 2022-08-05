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

        // ^^^ Low level API

        // vvv High level API

        const Guid& GetGuid() const noexcept
        {
            return GetHeader().Guid;
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

        std::span<const std::byte, 192> GetContext() const noexcept
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