#include "App.h"

#include "log/Log.h"
#include "resources/Resources.h"

#include <gtkmm/applicationwindow.h>

namespace L4
{
    App::App() :
        Gtk::Application("me.workingrobot.l4")
    {
    }

    Glib::RefPtr<App> App::create()
    {
        return Glib::make_refptr_for_instance<App>(new App());
    }

    void App::on_startup()
    {
        Gtk::Application::on_startup();

        Resources::GetResource()->register_global();

        Builder.emplace();

        SetupLogging();

        Modules.emplace();
        Modules->InitializeModules();

        auto& MainWindow = Builder->GetWidget<Gtk::ApplicationWindow>("MainWindow");
        add_window(MainWindow);
    }

    void App::on_activate()
    {
        Gtk::Application::on_activate();

        auto& MainWindow = Builder->GetWidget<Gtk::ApplicationWindow>("MainWindow");
        
        // I'm really really sorry for this jank, but if the window already exists, it won't grab
        // focus from the user unless it's been minimized for some reason.
        MainWindow.minimize();

        MainWindow.present();
    }

    int App::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
    {
        return Gtk::Application::on_command_line(command_line);
    }
}