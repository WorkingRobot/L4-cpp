#pragma once

#include "modules/base/ModuleList.h"
#include "manager/ClientInterface.h"
#include "manager/Plugin.h"

namespace L4::Modules::Plugins
{
    class Module : public Base::Module
    {
    public:
        Module(ModuleList& Ctx);

    private:
        Manager::ClientInterface Client;
        std::vector<Manager::Plugin> Plugins;
    };
}