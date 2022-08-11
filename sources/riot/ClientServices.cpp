#include "ClientServices.h"

#include "KnownFolders.h"

namespace L4::Riot
{
    ClientServices::ClientServices()
    {
    }

    std::optional<ClientServices::LockFileData> ClientServices::GetLockFileData()
    {
        auto LockFilePath = GetKnownFolderPath(FOLDERID_LocalAppData);
        if (LockFilePath.empty())
        {
            return std::nullopt;
        }

        LockFilePath /= "Riot Games";
        LockFilePath /= "Riot Client";
        LockFilePath /= "Config";
        LockFilePath /= "lockfile";

        if (std::error_code Code; !std::filesystem::is_regular_file(LockFilePath, Code))
        {
            return std::nullopt;
        }
        return std::nullopt;
    }
}