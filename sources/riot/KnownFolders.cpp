#include "KnownFolders.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <ShlObj_core.h>
#include <Windows.h>

namespace L4::Riot
{
    std::filesystem::path GetKnownFolderPath(const GUID& FolderId)
    {
        PWSTR RetPtr;
        if (SHGetKnownFolderPath(FolderId, 0, NULL, &RetPtr) != S_OK)
        {
            CoTaskMemFree(RetPtr);
            return "";
        }
        std::filesystem::path Ret = RetPtr;
        CoTaskMemFree(RetPtr);
        return Ret;
    }
}