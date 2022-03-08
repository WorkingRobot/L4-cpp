#pragma once

#include "Entry.h"

namespace L4
{
    class ArchiveHandle;

    class ArchiveView
    {
    public:
        ArchiveView() noexcept : BaseAddress(), Handle() {}

    private:
        ArchiveView(ArchiveHandle& Handle, void* BaseAddress) : BaseAddress(BaseAddress), Handle(&Handle) {}
        friend class ArchiveHandle;

    protected:
        void* GetClusterPtr(uint32_t ClusterIdx) noexcept
        {
            return (uint8_t*)BaseAddress + ClusterIdx * ClusterSize;
        }

        const void* GetClusterPtr(uint32_t ClusterIdx) const noexcept
        {
            return (const uint8_t*)BaseAddress + ClusterIdx * ClusterSize;
        }

        template<class T>
        T* GetClusterPtr(uint32_t ClusterIdx) noexcept
        {
            return (T*)GetClusterPtr(ClusterIdx);
        }

        template<class T>
        const T* GetClusterPtr(uint32_t ClusterIdx) const noexcept
        {
            return (const T*)GetClusterPtr(ClusterIdx);
        }

        void* GetSectorPtr(uint32_t SectorIdx) noexcept
        {
            return nullptr;
        }

        template<class T>
        T* GetSectorPtr(uint32_t SectorIdx) noexcept
        {
            return nullptr;
        }

    private:
        void* BaseAddress;
        ArchiveHandle* Handle;
    };
    static_assert(std::is_trivially_copyable_v<ArchiveView>);
}