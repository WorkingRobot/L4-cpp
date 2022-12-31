#pragma once

#include "modules/base/Module.h"
#include "manager/ClientInterface.h"
#include "manager/Plugin.h"

namespace L4::Modules::Plugins
{
    class Module : public Base::Module
    {
    public:
        Module();

    private:
        Manager::ClientInterface Client;
        std::vector<Manager::Plugin> Plugins;
    };
}