#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace L4
{
    // [Start, End]
    struct Interval
    {
        uint64_t Start;
        uint64_t End;
        const void* Buffer;
        size_t BufferSize;
    };

    class IntervalList
    {
    public:
        void Add(uint64_t Offset, uint64_t Size, const void* Buffer, size_t BufferSize)
        {
            Intervals.emplace_back(Offset, Offset + Size - 1, Buffer, BufferSize);
        }

        template <class T, size_t BufferSize>
        void Add(uint64_t Offset, uint64_t Size, const T(&Buffer)[BufferSize])
        {
            Add(Offset, Size, Buffer, BufferSize);
        }

        template<class T>
        void Add(uint64_t Offset, uint64_t Size, const T* Buffer)
        {
            Add(Offset, Size, Buffer, sizeof(T));
        }

        void Merge(uint64_t Offset, const IntervalList& List)
        {
            Intervals.reserve(Intervals.size() + List.Intervals.size());
            std::transform(List.Intervals.begin(), List.Intervals.end(), std::back_inserter(Intervals), [Offset](const auto& Int)
            {
                return Interval{ Int.Start + Offset, Int.End + Offset, Int.Buffer, Int.BufferSize };
            });
        }

    private:
        friend class IntervalTree;

        std::vector<Interval> Intervals;
    };

    class IntervalTree
    {
    public:
        IntervalTree() = default;

        IntervalTree(IntervalList&& IntervalList) :
            Data(std::move(IntervalList.Intervals))
        {
            std::sort(Data.begin(), Data.end(), [](const auto& A, const auto& B)
            {
                return A.Start < B.Start;
            });
        }

        template <class FuncT>
        void Get(uint64_t Idx, uint64_t Size, FuncT Func)
        {
            auto LowerItr =
                std::lower_bound(Data.begin(), Data.end(), Idx, [](const auto& Interval, uint64_t Idx)
            {
                return Interval.End < Idx;
            });
            auto UpperItr =
                std::upper_bound(LowerItr, Data.end(), Idx + Size, [](uint64_t Idx, const auto& Interval)
            {
                return Idx <= Interval.Start;
            });

            std::for_each(LowerItr, UpperItr, Func);
        }

    private:
        std::vector<Interval> Data;
    };
}