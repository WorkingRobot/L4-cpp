#pragma once

#include "modules/base/ModuleList.h"

namespace L4
{
    class ModuleList : public Modules::ModuleList
    {
    public:
        using Modules::ModuleList::ModuleList;

        void InitializeModules();
    };
}