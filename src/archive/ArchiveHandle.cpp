#include "ArchiveHandle.h"

namespace L4
{
    ArchiveHandle::ArchiveHandle(void* BaseAddress) :
        BaseAddress(BaseAddress)
    {

    }

    ArchiveHandle::operator ArchiveView() noexcept
    {
        return ArchiveView(*this, BaseAddress);
    }
}