#pragma once

#include <giomm/resource.h>

namespace L4::Resources
{
    Glib::RefPtr<Gio::Resource> GetResource();
}