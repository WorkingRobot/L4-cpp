#include "ModuleList.h"

#include "log/Log.h"
#include "modules/plugins/Module.h"
#include "utils/Stopwatch.h"

#include <type_name/type_name.hpp>

namespace L4
{
    void ModuleList::InitializeModules()
    {
        AddModule<Modules::Plugins::Module>();
    }

    template <class T>
    static consteval std::string_view GetModuleNamespace()
    {
        auto Name = type_name_v<Modules::Plugins::Module>;
        std::string_view Begin = "class L4::Modules::";
        if (Name.starts_with(Begin))
        {
            Name.remove_prefix(Begin.size());
        }
        std::string_view End = "::Module";
        if (Name.ends_with(End))
        {
            Name.remove_suffix(End.size());
        }
        return Name;
    }

    template <class T>
    void ModuleList::AddModule()
    {
        auto Timer = Stopwatch::StartNew();
        Modules::ModuleList::AddModule<T>();
        Timer.Stop();
        LOG(Info, "{:s} module loaded in {:.2f} ms", GetModuleNamespace<T>(), Timer.TimeElapsedMs());
    }
}