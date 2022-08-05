#pragma once

#include "Base.h"

extern "C" {
    L4_API uint32_t GetSourceVersion();

    L4_API void* InitializeSource(uint32_t Version);

    L4_API void UninitializeSource(void* Source);
}
