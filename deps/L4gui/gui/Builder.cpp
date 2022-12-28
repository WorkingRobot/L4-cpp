#include "Builder.h"

namespace L4::Gui
{
    Builder::Builder() :
        BuilderImpl(Gtk::Builder::create_from_resource("/ui/L4.ui"))
    {
    }
}
