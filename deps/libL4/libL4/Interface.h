#pragma once

#include "Structs.h"

#if defined(_WIN32)
#define L4_EXPORT extern "C" __declspec(dllexport)
#else
#define L4_EXPORT extern "C" __attribute__((visibility("default")))
#endif

L4_EXPORT bool Initialize(const libL4::ClientInterface* Interface, libL4::PluginInterface* OutInterface);