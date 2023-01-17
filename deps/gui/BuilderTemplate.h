#pragma once

#include <gtkmm/builder.h>
#include <gtkmm/listitem.h>

namespace L4::Gui
{
    class BuilderTemplate
    {
    public:
        BuilderTemplate(const Glib::ustring& ResourcePath);

        Glib::RefPtr<Gtk::Builder> Setup(const Glib::RefPtr<Gtk::ListItem>& Item);

    private:
        Glib::RefPtr<Glib::Bytes> Bytes;
        gconstpointer Data;
        gsize Size;
    };
}