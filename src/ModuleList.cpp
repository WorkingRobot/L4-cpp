#include "ModuleList.h"

#include "modules/plugins/Module.h"

namespace L4
{
    void ModuleList::InitializeModules()
    {
        AddModule<Modules::Plugins::Module>();
    }
}