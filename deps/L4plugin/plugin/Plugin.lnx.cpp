#include "Plugin.h"

#include <dlfcn.h>
#include <utils/Error.lnx.h>

namespace L4::Plugin::Manager
{
    Plugin::Plugin(const std::filesystem::path& FilePath, const libL4::Marshal::ClientIdentity& ClientIdentity, const std::filesystem::path& ConfigBaseDirectory) :
        ClientIdentity(ClientIdentity)
    {
        ModuleHandle = dlopen(FilePath.c_str(), RTLD_LAZY);
        if (ModuleHandle == nullptr)
        {
            throw CreateErrorCustom(EINVAL, dlerror());
        }

        auto InitializerProc = (GetInitializerProc)dlsym(ModuleHandle, "GetInitializer");
        if (InitializerProc == NULL)
        {
            throw CreateErrorCustom(EINVAL, dlerror());
        }

        InitializeProc = InitializerProc();
    }
}