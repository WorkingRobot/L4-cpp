#include "SourceLibrary.h"

#include "utils/Error.lnx.h"

#include <dlfcn.h>

namespace L4
{
    SourceLibrary::SourceLibrary(const std::filesystem::path& FilePath)
    {
        ModuleHandle = dlopen(FilePath.c_str(), RTLD_LAZY);
        if (ModuleHandle == nullptr)
        {
            throw CreateErrorCustom(EINVAL, dlerror());
        }
        InitProc = (InitializeProc)dlsym(ModuleHandle, "Initialize");
        if (InitProc == NULL)
        {
            throw CreateErrorCustom(EINVAL, dlerror());
        }
    }

    SourceLibrary::~SourceLibrary()
    {
        dlclose(ModuleHandle);
    }
}