#pragma once

#include "Structs.h"

#if defined(_WIN32)
#if defined(L4_SOURCE_BUILDING_SOURCE)
#define L4_SOURCE_EXPORT extern "C" __declspec(dllexport)
#else
#define L4_SOURCE_EXPORT extern "C" __declspec(dllimport)
#endif
#else
#define L4_SOURCE_EXPORT extern "C" __attribute__((visibility("default")))
#endif

L4_SOURCE_EXPORT const libL4::PluginInterface* Initialize(const libL4::ClientInterface* Interface);