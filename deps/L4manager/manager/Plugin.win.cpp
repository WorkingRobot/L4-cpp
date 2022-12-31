#include "Plugin.h"

#include "utils/Error.win.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4::Manager
{
    void* Plugin::LoadPlugin(const std::filesystem::path& Path)
    {
        auto OSHandle = LoadLibraryW(Path.c_str());
        if (OSHandle == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        return OSHandle;
    }

    decltype(&::GetInitializer) Plugin::GetInitializerFunc(void* OSHandle)
    {
        auto GetInitializerProc = (decltype(&GetInitializer))(GetProcAddress((HMODULE)OSHandle, "GetInitializer"));
        if (GetInitializerProc == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        return GetInitializerProc;
    }

    void Plugin::UnloadPlugin(void* OSHandle)
    {
        FreeLibrary((HMODULE)OSHandle);
    }
}