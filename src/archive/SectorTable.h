#pragma once

#include "ArchiveView.h"

namespace L4
{
    class ClusterBitmap : ArchiveView
    {
        struct Section
        {
            uint8_t Data[65536];
        };

        static auto SplitIdx(uint32_t ClusterIdx)
        {
            union U
            {
                uint32_t In;
                struct
                {
                    uint32_t BitIdx : 3;
                    uint32_t ByteIdx : 16;
                    uint32_t TableIdx : 13;
                } Out;
            };
            return U{ .In = ClusterIdx }.Out;
        }

    public:
        bool Lookup(uint32_t ClusterIdx) const noexcept
        {
            auto [BitIdx, ByteIdx, TableIdx] = SplitIdx(ClusterIdx);
            if (Table[TableIdx] == InvalidIdx)
            {
                return false;
            }
            return (GetClusterPtr<Section>(Table[TableIdx])->Data[ByteIdx] >> BitIdx) & 1u;
        }

        [[nodiscard]] uint32_t Allocate()
        {
            auto RetClusterIdx = GetNextFreeCluster();
            if (RetClusterIdx == InvalidIdx)
            {
                throw std::bad_alloc{};
            }

            auto [BitIdx, ByteIdx, TableIdx] = SplitIdx(RetClusterIdx);
            if (Table[TableIdx] == InvalidIdx)
            {
                auto TableClusterIdx = GetNextFreeCluster(RetClusterIdx + 1);
                if (TableClusterIdx == InvalidIdx)
                {
                    throw std::bad_alloc{};
                }

                Table[TableIdx] = TableClusterIdx;
            }

            GetClusterPtr<Section>(Table[TableIdx])->Data[ByteIdx] |= 1u << BitIdx;
            return RetClusterIdx;
        }

        void Free(uint32_t ClusterIdx) noexcept
        {
            if (!Lookup(ClusterIdx))
            {
                return;
            }

            auto [BitIdx, ByteIdx, TableIdx] = SplitIdx(ClusterIdx);
            GetClusterPtr<Section>(Table[TableIdx])->Data[ByteIdx] &= ~(1u << BitIdx);
        }

    private:
        uint32_t GetNextFreeCluster(uint32_t StartIdx = 1) const noexcept
        {
            for (uint32_t Idx = StartIdx; Idx <= 0xFFFFFFFF; ++Idx)
            {
                if (!Lookup(Idx))
                {
                    return Idx;
                }
            }
            return InvalidIdx;
        }

        uint32_t Table[8192];
        uint32_t Reserved[8192];
    };
}