#include "Module.h"

#include "gui/ListContainerModel.h"
#include "utils/Config.h"

#include <gtkmm/label.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/singleselection.h>

namespace L4::Modules::Plugins
{
    Glib::RefPtr<Glib::Bytes> GetBytes()
    {
        struct _GtkListItemFactory
        {
            GObject parent_instance;
        };

        struct _GtkBuilderListItemFactory
        {
            _GtkListItemFactory parent_instance;
            GtkBuilderScope* scope;
            GBytes* bytes;
            GBytes* data;
            char* resource;
        };

        auto Factory = (GtkBuilderListItemFactory*)g_object_new(GTK_TYPE_BUILDER_LIST_ITEM_FACTORY, "resource", "/ui/PluginListRow.ui", NULL);
        auto FactoryInternal = (_GtkBuilderListItemFactory*)Factory;

        auto Bytes = Glib::wrap(g_bytes_ref(FactoryInternal->data));
        g_object_unref(Factory);

        return Bytes;

        // _gtk_buildable_parser_precompile doesn't exist publicly, so we indirectly call it through GtkBuilderListItemFactory.
        // When its resource property gets set, it gets precompiled on initialization, and we can grab the precompiled data that way.
        // The data variable is never publicly exposed, so we must grab it through internal means. I doubt the ABI will break anytime soon, however.

        /*
        auto Resource = Gio::Resource::lookup_data_global("/ui/PluginListRow.ui");
        gsize Size = 0;
        gconstpointer Data = Resource->get_data(Size);

        GError* Error = NULL;
        auto RetPtr = _gtk_buildable_parser_precompile((const char*)Data, Size, &Error); // <<< _gtk_buildable_parser_precompile is not publicly exposed
        if (Data == NULL)
        {
            g_warning("Failed to precompile template for GtkBuilderListItemFactory: %s", Error->message);
            g_error_free(Error);
            return nullptr;
        }
        return Glib::wrap(RetPtr);
        */
    }

    // gtk_builder_list_item_factory_setup
    void SetupItem(const Glib::RefPtr<Gtk::ListItem>& Item)
    {
        auto Builder = Gtk::Builder::create();

        static auto Bytes = GetBytes();
        static gsize BytesSize = 0;
        static gconstpointer BytesData = Bytes->get_data(BytesSize);

        Builder->expose_object(g_type_name(Item->get_type()), Item);
        GError* Error = NULL;
        if (!gtk_builder_extend_with_template(Builder->gobj(), G_OBJECT(Item->gobj()), G_OBJECT_TYPE(Item->gobj()), (const char*)BytesData, BytesSize, &Error))
        {
            g_critical("Error building template for list item: %s", Error->message);
            g_error_free(Error);
        }
        
        auto Widget = Builder->get_object<Gtk::Label>("PluginName");
        Widget->set_label("among us");
    }

    Module::Module(ModuleList& Ctx) :
        AppView(Ctx.GetWidget<Gtk::ListView>("AppView")),
        PluginView(Ctx.GetWidget<Gtk::ListView>("PluginView")),
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
        Factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& Item) {
            SetupItem(Item);
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