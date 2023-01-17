#include "ModuleList.h"

#include <algorithm>
#include <ranges>

namespace L4::Modules
{
    ModuleList::ModuleList() :
        Store("settings.bin")
    {
    }

    ModuleList::~ModuleList()
    {
        // Delete in reverse to perserve dependencies
        // std::vector doesn't guarantee reverse destruction like std::array does
        std::ranges::for_each(std::views::reverse(Modules), [](auto& Module) {
            auto Timer = Stopwatch::StartNew();
            Module.second.reset();
            Timer.Stop();
            LOG(Info, "{:s} module unloaded in {:.2f} ms", Module.first, Timer.TimeElapsedMs());
        });
    }
}