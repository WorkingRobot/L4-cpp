#pragma once

#include "../tree/IntervalTree.h"
#include <memory>

namespace L4
{
    using ExFatSystemPrivate = class ExFatSystem;
    class ExFatSystemPublic
    {
    public:
        ExFatSystemPublic();

        ~ExFatSystemPublic();

        const IntervalList& GetIntervalList() const;

    private:
        std::unique_ptr<ExFatSystemPrivate> Private;
    };
}