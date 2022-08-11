#pragma once

#include <guiddef.h>
#include <KnownFolders.h>

#include <filesystem>

namespace L4::Riot
{
    std::filesystem::path GetKnownFolderPath(const GUID& FolderId);
}