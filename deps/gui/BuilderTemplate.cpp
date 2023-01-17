#include "BuilderTemplate.h"

namespace L4::Gui
{
    BuilderTemplate::BuilderTemplate(const Glib::ustring& ResourcePath)
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

        // _gtk_buildable_parser_precompile doesn't exist publicly, so we indirectly call it through GtkBuilderListItemFactory.
        // When its resource property gets set, it gets precompiled on initialization, and we can grab the precompiled data that way.
        // The data variable is never publicly exposed, so we must grab it through internal means. I doubt the ABI will break anytime soon anyway.

        auto Factory = (GtkBuilderListItemFactory*)g_object_new(GTK_TYPE_BUILDER_LIST_ITEM_FACTORY, "resource", ResourcePath.c_str(), NULL);
        Bytes = Glib::wrap(((_GtkBuilderListItemFactory*)Factory)->data, true);
        g_object_unref(Factory);

        Data = Bytes->get_data(Size);
    }

    Glib::RefPtr<Gtk::Builder> BuilderTemplate::Setup(const Glib::RefPtr<Gtk::ListItem>& Item)
    {
        // gtk_builder_list_item_factory_setup
        auto Builder = Gtk::Builder::create();
        
        GError* Error = NULL;
        if (!gtk_builder_extend_with_template(Builder->gobj(), G_OBJECT(Item->gobj()), G_OBJECT_TYPE(Item->gobj()), (const char*)Data, Size, &Error))
        {
            g_critical("Error building template for list item: %s", Error->message);
            g_error_free(Error);
        }

        return Builder;
    }
}
