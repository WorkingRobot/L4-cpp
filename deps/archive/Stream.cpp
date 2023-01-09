#include "Stream.h"

#include "Freelist.h"

namespace L4::Archive
{
    Stream::Stream(const Archive& Ar, uint32_t Idx) :
        Ar(Ar),
        Idx(Idx)
    {
    }

    bool Stream::IsValid() const noexcept
    {
        return GetHeader().Id[0] != '\0';
    }

    const StreamHeader& Stream::GetHeader() const noexcept
    {
        return *Ar.File.Get<StreamHeader>((Idx + 1LL) * sizeof(StreamHeader));
    }

    uint64_t Stream::GetCapacity() const noexcept
    {
        return GetCapacityInSectors() * (uint64_t)Ar.SectorSize;
    }

    uint64_t Stream::GetSize() const noexcept
    {
        return GetRunlist().Size;
    }

    void Stream::ReadBytes(uint64_t Offset, std::span<std::byte> Dst) const
    {
        IterateRangeInternal(Offset, Dst.size(), [&Dst](std::span<const std::byte> Data) noexcept {
            std::ranges::copy(Data, Dst.begin());
            Dst = Dst.subspan(Data.size());
        });
    }

    template <class FuncT>
    inline void Stream::IterateRangeInternal(uint64_t Offset, uint64_t Size, FuncT Func) const
    {
        const StreamRunlist& Runlist = GetRunlist();

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

            const auto RunData = Ar.GetSectors(RunItr->SectorOffset, RunItr->SectorCount);
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

    const StreamRunlist& Stream::GetRunlist() const noexcept
    {
        return *Ar.File.Get<StreamRunlist>(Ar.FreelistOffset + sizeof(Freelist) + Idx * sizeof(StreamRunlist));
    }

    uint32_t Stream::GetCapacityInSectors() const noexcept
    {
        const StreamRunlist& Runlist = GetRunlist();
        if (Runlist.RunCount == 0)
        {
            return 0;
        }
        const StreamRunlist::Run& LastRun = Runlist.Runs[Runlist.RunCount - 1];
        return LastRun.StreamSectorOffset + LastRun.SectorCount;
    }
}