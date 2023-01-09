#include "StreamWritable.h"

#include "Freelist.h"
#include "utils/Align.h"

namespace L4::Archive
{
    StreamWritable::StreamWritable(ArchiveWritable& Ar, uint32_t Idx) :
        Ar(Ar),
        Idx(Idx)
    {
    }

    bool StreamWritable::IsValid() const
    {
        std::shared_lock Lock(Ar.HeadersMtx);

        return IsValidUnlocked();
    }

    StreamHeader StreamWritable::GetHeader() const
    {
        std::shared_lock Lock(Ar.HeadersMtx);

        return GetHeaderUnlocked();
    }
    
    uint64_t StreamWritable::GetCapacity() const
    {
        std::shared_lock Lock(Ar.RunlistsMtx);

        return GetCapacityInSectorsUnlocked() * (uint64_t)Ar.SectorSize;
    }

    uint64_t StreamWritable::GetSize() const
    {
        std::shared_lock Lock(Ar.RunlistsMtx);

        return GetRunlistUnlocked().Size;
    }

    void StreamWritable::SetHeader(const StreamHeader& NewHeader)
    {
        std::lock_guard Lock(Ar.HeadersMtx);

        SetHeaderUnlocked(NewHeader);
    }

    static constexpr auto MinHeapEntries = [](const Freelist::Entry& A, const Freelist::Entry& B) noexcept {
        return A.SectorCount > B.SectorCount;
    };

    void StreamWritable::ReserveCapacity(uint64_t NewCapacity)
    {
        std::lock_guard Lock(Ar.RunlistsMtx);

        ReserveCapacityUnlocked(NewCapacity);
    }

    void StreamWritable::ShrinkToFit()
    {
        std::lock_guard Lock(Ar.RunlistsMtx);

        ShrinkToFitUnlocked();
    }

    void StreamWritable::Resize(uint64_t NewSize)
    {
        std::lock_guard Lock(Ar.RunlistsMtx);

        ResizeUnlocked(NewSize);
    }

    void StreamWritable::WriteBytes(uint64_t Offset, std::span<const std::byte> Src)
    {
        std::shared_lock Lock(Ar.RunlistsMtx);

        IterateRangeUnlocked(Offset, Src.size(), [&Src](std::span<std::byte> Data) noexcept {
            std::ranges::copy(Src.subspan(0, Data.size()), Data.begin());
            Src = Src.subspan(Data.size());
        });
    }

    void StreamWritable::ReadBytes(uint64_t Offset, std::span<std::byte> Dst) const
    {
        std::shared_lock Lock(Ar.RunlistsMtx);

        IterateRangeUnlocked(Offset, Dst.size(), [&Dst](std::span<const std::byte> Data) noexcept {
            std::ranges::copy(Data, Dst.begin());
            Dst = Dst.subspan(Data.size());
        });
    }

    template <class FuncT>
    inline void StreamWritable::IterateRangeUnlocked(uint64_t Offset, uint64_t Size, FuncT Func) const
    {
        const StreamRunlist& Runlist = GetRunlistUnlocked();

        if (Offset + Size > Runlist.Size)
        {
            throw std::out_of_range("Operation will go out of bounds");
        }

        // Sector offset within the stream
        const uint32_t StreamSectorIdx = Offset / Ar.SectorSize;
        // Byte offset within the current sector
        uint32_t SectorOffset = Offset % Ar.SectorSize;
        auto RunItr = std::lower_bound(Runlist.Runs, Runlist.Runs + Runlist.RunCount, StreamSectorIdx, [](const StreamRunlist::Run& Interval, uint32_t Idx) {
            return Interval.StreamSectorOffset + Interval.SectorCount <= Idx;
        });
        if (RunItr == Runlist.Runs + Runlist.RunCount)
        {
            throw std::out_of_range("Starting operation from out of bounds");
        }

        // Index of the current sector within the current run
        uint32_t RunSectorIdx = StreamSectorIdx - RunItr->StreamSectorOffset;

        do
        {
            const uint32_t ByteCount = std::min<uint32_t>((RunItr->SectorCount - RunSectorIdx) * Ar.SectorSize - SectorOffset, Size);

            const auto RunData = Ar.GetSectorsUnlocked(RunItr->SectorOffset, RunItr->SectorCount);
            const auto Data = RunData.subspan(RunSectorIdx * (size_t)Ar.SectorSize + SectorOffset, ByteCount);
            Func(Data);

            Size -= ByteCount;
            if (Size == 0)
            {
                return;
            }

            // Because we either finish the operation or we operate to the end of a whole run,
            // we can assume that we will begin at the start of a new run for all subsequent iterations
            SectorOffset = 0;
            RunSectorIdx = 0;
        } while (++RunItr != Runlist.Runs + Runlist.RunCount);

        throw std::out_of_range("Continuing operation from out of bounds");
    }

    void StreamWritable::ReserveCapacityUnlocked(uint64_t NewCapacity)
    {
        const uint32_t RequestedCapacity = Align(NewCapacity, Ar.SectorSize) / Ar.SectorSize;
        uint32_t CurrentCapacity = GetCapacityInSectorsUnlocked();

        if (CurrentCapacity >= RequestedCapacity)
        {
            return;
        }

        uint32_t MoreSectorsNeeded = RequestedCapacity - CurrentCapacity;

        Freelist& Freelist = Ar.GetFreelistUnlocked();
        StreamRunlist& Runlist = GetRunlistUnlocked();

        // Extend current run and extend archive
        if (Runlist.RunCount != 0)
        {
            StreamRunlist::Run& LastRun = Runlist.Runs[Runlist.RunCount - 1];
            // If the last run for this stream touches the end of the archive, we can extend as far as we want
            if (LastRun.SectorOffset + LastRun.SectorCount == Freelist.TotalSectorCount)
            {
                Freelist.TotalSectorCount += MoreSectorsNeeded;
                Ar.File.Reserve(Freelist.TotalSectorCount * size_t(Ar.SectorSize));
                LastRun.SectorCount += MoreSectorsNeeded;
                return;
            }
        }

        // Utilize freelist
        while (Freelist.EntryCount != 0)
        {
            std::pop_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
            Freelist::Entry& LastEntry = Freelist.Entries[Freelist.EntryCount - 1];
            if (LastEntry.SectorCount > MoreSectorsNeeded)
            {
                Runlist.Runs[Runlist.RunCount++] = {
                    .StreamSectorOffset = CurrentCapacity,
                    .SectorCount = MoreSectorsNeeded,
                    .SectorOffset = LastEntry.SectorOffset,
                };

                LastEntry.SectorCount -= MoreSectorsNeeded;
                LastEntry.SectorOffset += MoreSectorsNeeded;

                std::push_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
                return;
            }
            else
            {
                Runlist.Runs[Runlist.RunCount++] = {
                    .StreamSectorOffset = CurrentCapacity,
                    .SectorCount = LastEntry.SectorCount,
                    .SectorOffset = LastEntry.SectorOffset,
                };

                MoreSectorsNeeded -= LastEntry.SectorCount;
                CurrentCapacity += LastEntry.SectorCount;

                LastEntry = {};
                Freelist.EntryCount--;
            }
        }

        // Create new run and extend archive
        Runlist.Runs[Runlist.RunCount++] = {
            .StreamSectorOffset = CurrentCapacity,
            .SectorCount = MoreSectorsNeeded,
            .SectorOffset = Freelist.TotalSectorCount,
        };
        Freelist.TotalSectorCount += MoreSectorsNeeded;
        Ar.File.Reserve(Freelist.TotalSectorCount * size_t(Ar.SectorSize));
    }

    void StreamWritable::ShrinkToFitUnlocked()
    {
        Freelist& Freelist = Ar.GetFreelistUnlocked();
        StreamRunlist& Runlist = GetRunlistUnlocked();

        const uint32_t TargetCapacity = Align(Runlist.Size, Ar.SectorSize) / Ar.SectorSize;
        uint32_t CurrentCapacity = GetCapacityInSectorsUnlocked();

        uint32_t SectorsToFree = CurrentCapacity - TargetCapacity;
        while (SectorsToFree != 0)
        {
            StreamRunlist::Run& Run = Runlist.Runs[Runlist.RunCount - 1];
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

                Run = {};
                Runlist.RunCount--;

                std::push_heap(Freelist.Entries, Freelist.Entries + Freelist.EntryCount, MinHeapEntries);
            }
        }
    }

    void StreamWritable::ResizeUnlocked(uint64_t NewSize)
    {
        StreamRunlist& Runlist = GetRunlistUnlocked();
        if (NewSize <= Runlist.Size)
        {
            Runlist.Size = NewSize;
        }
        else
        {
            ReserveCapacityUnlocked(NewSize);
            Runlist.Size = NewSize;
        }
    }

    void StreamWritable::DeleteUnlocked()
    {
        ResizeUnlocked(0);
        ShrinkToFitUnlocked();
        SetHeaderUnlocked({});
    }

    uint32_t StreamWritable::GetCapacityInSectorsUnlocked() const noexcept
    {
        const StreamRunlist& Runlist = GetRunlistUnlocked();
        if (Runlist.RunCount == 0)
        {
            return 0;
        }
        const StreamRunlist::Run& LastRun = Runlist.Runs[Runlist.RunCount - 1];
        return LastRun.StreamSectorOffset + LastRun.SectorCount;
    }

    bool StreamWritable::IsValidUnlocked() const noexcept
    {
        // Id is a null terminated string (unless it's 32 bytes long). If and only
        // if it's first byte is 0, then it's an empty string and therefore invalid.
        return GetHeaderUnlocked().Id[0] != '\0';
    }

    StreamHeader& StreamWritable::GetHeaderUnlocked() noexcept
    {
        return *Ar.File.Get<StreamHeader>((Idx + 1LL) * sizeof(StreamHeader));
    }

    const StreamHeader& StreamWritable::GetHeaderUnlocked() const noexcept
    {
        return *Ar.File.Get<StreamHeader>((Idx + 1LL) * sizeof(StreamHeader));
    }

    void StreamWritable::SetHeaderUnlocked(const StreamHeader& NewHeader) noexcept
    {
        StreamHeader& OldHeader = GetHeaderUnlocked();

        OldHeader = NewHeader;
    }

    StreamRunlist& StreamWritable::GetRunlistUnlocked() noexcept
    {
        return *Ar.File.Get<StreamRunlist>(Ar.FreelistOffset + sizeof(Freelist) + Idx * sizeof(StreamRunlist));
    }

    const StreamRunlist& StreamWritable::GetRunlistUnlocked() const noexcept
    {
        return *Ar.File.Get<StreamRunlist>(Ar.FreelistOffset + sizeof(Freelist) + Idx * sizeof(StreamRunlist));
    }
}