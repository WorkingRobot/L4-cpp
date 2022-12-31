#include "ModuleList.h"

#include <algorithm>
#include <ranges>

namespace L4::Modules
{
    ModuleList::ModuleList()
    {
    }

    ModuleList::~ModuleList()
    {
        // Delete in reverse to perserve dependencies
        // std::vector doesn't guarantee reverse destruction like std::array does
        std::ranges::for_each(std::views::reverse(Modules), [](auto& Module) { Module.reset(); });
    }
}