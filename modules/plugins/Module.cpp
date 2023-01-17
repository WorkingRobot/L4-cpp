#include "Module.h"

#include "gui/ListContainerModel.h"
#include "utils/Config.h"

#include <gtkmm/label.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/singleselection.h>

namespace L4::Modules::Plugins
{
    Module::Module(ModuleList& Ctx) :
        AppView(Ctx.GetWidget<Gtk::ListView>("AppView")),
        PluginView(Ctx.GetWidget<Gtk::ListView>("PluginView")),
        PluginTemplate("/ui/PluginListRow.ui"),
        Client({
                   .Id = u8"l4",
                   .Name = u8"L4",
                   .Version = (char8_t*)Config::GetVersionLong(),
                   .VersionNumeric = Config::GetVersionNumeric(),
               },
               Config::GetFolder())
    {
        for (const auto& Entry : std::filesystem::directory_iterator(Config::GetFolder() / "plugins"))
        {
            auto Filename = Entry.path().filename().string();
            if (Filename.starts_with("L4-plugin-") && Filename.ends_with(".dll"))
            {
                auto& Plugin = Plugins.emplace_back(Entry);
                Plugin.OnAuthUserUpdated = []() {};
                Plugin.Initialize(Client);
            }
        }

        for (const auto& Entry : std::filesystem::directory_iterator(Config::GetFolder() / "archives"))
        {
            auto Filename = Entry.path().filename().string();
            if (Filename.ends_with(".l4a"))
            {
                Apps.emplace_back(Entry);
            }
        }

        auto Factory = Gtk::SignalListItemFactory::create();
        Factory->signal_setup().connect([this](const Glib::RefPtr<Gtk::ListItem>& Item) {
            auto Builder = PluginTemplate.Setup(Item);

            auto Widget = Builder->get_object<Gtk::Label>("PluginName");
            Widget->set_label("among us");
        });
        Factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem>& Item) {
            auto ItemPtr = Item->get_item();
        });
        Factory->signal_unbind().connect([](const Glib::RefPtr<Gtk::ListItem>& Item) {
            auto ItemPtr = Item->get_item();
        });
        Factory->signal_teardown().connect([](const Glib::RefPtr<Gtk::ListItem>& Item) {
            auto ItemPtr = Item->get_item();
            Item->unset_child();
        });
        PluginView.set_factory(Factory);
        PluginView.set_model(Gtk::SingleSelection::create(Gui::ListContainerModel<decltype(Plugins)>::create(Plugins)));
    }

    Module::~Module()
    {
        // Workaround for https://gitlab.gnome.org/GNOME/gtk/-/issues/4931
        // Otherwise, when Gtk::Builder destructs, it throws an
        // assert, and we don't want a crash when app closes
        PluginView.set_model(nullptr);
    }
}