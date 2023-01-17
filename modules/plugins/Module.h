#pragma once

#include "gui/BuilderTemplate.h"
#include "manager/App.h"
#include "manager/ClientInterface.h"
#include "manager/Plugin.h"
#include "modules/base/ModuleList.h"

#include <gtkmm/listview.h>

namespace L4::Modules::Plugins
{
    class Module : public Base::Module
    {
    public:
        Module(ModuleList& Ctx);

        ~Module();

    private:
        Gtk::ListView& AppView;
        Gtk::ListView& PluginView;
        Gui::BuilderTemplate PluginTemplate;

        Manager::ClientInterface Client;
        std::vector<Manager::Plugin> Plugins;
        std::vector<Manager::App> Apps;
    };
}