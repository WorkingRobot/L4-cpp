#pragma once

#include <algorithm>
#include <iterator>
#include <span>
#include <vector>

namespace L4 {
    // [Start, End]
    struct Interval {
        uint64_t Start;
        uint64_t End;
        std::span<const std::byte> Buffer;
    };

    class IntervalList {
    public:
        void Add(uint64_t Offset, uint64_t Size, std::span<const std::byte> Buffer)
        {
            Intervals.emplace_back(Interval { Offset, Offset + Size - 1, Buffer });
        }

        template <class T, size_t Extent>
        void Add(uint64_t Offset, uint64_t Size, std::span<T, Extent> Buffer)
        {
            Add(Offset, Size, std::as_bytes(Buffer));
        }

        void Merge(uint64_t Offset, const IntervalList& List)
        {
            Intervals.reserve(Intervals.size() + List.Intervals.size());
            std::transform(List.Intervals.begin(), List.Intervals.end(), std::back_inserter(Intervals), [Offset](const auto& Int) {
                return Interval { Int.Start + Offset, Int.End + Offset, Int.Buffer };
            });
        }

    private:
        friend class IntervalTree;

        std::vector<Interval> Intervals;
    };

    class IntervalTree {
    public:
        IntervalTree() = default;

        IntervalTree(IntervalList&& IntervalList) :
            Data(std::move(IntervalList.Intervals))
        {
            std::ranges::sort(Data, {}, [](const Interval& Int) { return Int.Start; });
        }

        template <class FuncT>
        void Get(uint64_t Idx, uint64_t Size, FuncT Func)
        {
            std::for_each(
                std::ranges::lower_bound(Data, Idx, std::less {}, [](const Interval& Int) { return Int.End; }),
                std::ranges::upper_bound(Data, Idx + Size, std::less_equal {}, [](const Interval& Int) { return Int.Start; }),
                Func);
        }

    public:
        std::vector<Interval> Data;
    };
}