#pragma once

#include "Structs.h"

#if defined(_WIN32)
#define L4_EXPORT extern "C" __declspec(dllexport)
#else
#define L4_EXPORT extern "C" __attribute__((visibility("default")))
#endif

namespace libL4
{
    using Initializer = void (*)(libL4::Handle ClientHandle, const libL4::ClientInterface* Interface, libL4::PluginInterface* OutInterface);
}

L4_EXPORT libL4::Initializer GetInitializer();