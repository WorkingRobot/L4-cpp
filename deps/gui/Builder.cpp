#include "Builder.h"

namespace L4::Gui
{
    Builder::Builder() :
        Impl(Gtk::Builder::create_from_resource("/ui/L4.ui"))
    {
    }
}
