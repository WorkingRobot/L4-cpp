#pragma once

#include <gtkmm/builder.h>

namespace L4::Gui
{
    class Builder
    {
    public:
        Builder();

        template <class T>
        [[nodiscard]] T& GetWidget(const Glib::ustring& Name) const
        {
            auto Ptr = Impl->get_widget<T>(Name);
            if (!Ptr) [[unlikely]]
            {
                throw std::runtime_error("Widget does not exist.");
            }
            return *Ptr;
        }

    private:
        Glib::RefPtr<Gtk::Builder> Impl;
    };
}