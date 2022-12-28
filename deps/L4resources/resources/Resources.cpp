#include "Resources.h"

extern "C" {
extern GResource* L4_get_resource(void);
}

namespace L4::Resources
{
    Glib::RefPtr<Gio::Resource> GetResource()
    {
        return Glib::wrap(L4_get_resource());
    }
}