#include "SourceLibrary.h"

#include "utils/Error.win.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    SourceLibrary::SourceLibrary(const std::filesystem::path& FilePath)
    {
        ModuleHandle = LoadLibraryW(FilePath.c_str());
        if (ModuleHandle == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        InitProc = (InitializeProc)GetProcAddress((HMODULE)ModuleHandle, "Initialize");
        if (InitProc == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
    }

    SourceLibrary::~SourceLibrary()
    {
        FreeLibrary((HMODULE)ModuleHandle);
    }
}