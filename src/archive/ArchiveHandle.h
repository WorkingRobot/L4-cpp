#pragma once

#include "ArchiveView.h"
#include "ClusterBitmap.h"

namespace L4
{
    class ArchiveHandle
    {
    public:
        explicit ArchiveHandle(void* BaseAddress);
        ArchiveHandle(const ArchiveHandle&) = delete;
        ArchiveHandle(ArchiveHandle&&) = delete;

        operator ArchiveView() noexcept;

    private:
        void* BaseAddress;
    };
}