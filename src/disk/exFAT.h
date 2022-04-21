#pragma once

#include "../tree/IntervalTree.h"
#include <memory>

namespace L4
{
    using ExFatSystemPrivate = class ExFatSystem;
    class ExFatSystemPublic
    {
    public:
        ExFatSystemPublic(uint64_t PartitionSectorOffset, uint64_t PartitionSectorCount);

        ~ExFatSystemPublic();

        const IntervalList& GetIntervalList() const;

    private:
        std::unique_ptr<ExFatSystemPrivate> Private;
    };
}