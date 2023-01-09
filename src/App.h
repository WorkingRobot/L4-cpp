#pragma once

#include "gui/Builder.h"
#include "ModuleList.h"

#include <gtkmm/application.h>

#include <optional>

namespace L4
{
    class App : public Gtk::Application
    {
    protected:
        App();

    public:
        static Glib::RefPtr<App> create();

    protected:
        void on_startup() override;

        void on_activate() override;

        int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) override;

    private:
        std::optional<ModuleList> Modules;
    };
}