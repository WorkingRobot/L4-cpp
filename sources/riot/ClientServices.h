#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace L4::Riot
{
    class ClientServices
    {
    public:
        ClientServices();

        bool IsRunning();

    public:
        struct LockFileData
        {
            std::string Name;
            uint32_t Pid;
            uint16_t Port;
            std::string PrivateToken;
            std::string Scheme;
        };

        static std::optional<LockFileData> GetLockFileData();
        
        static std::filesystem::path GetLockFilePath();

        static void WaitForLockFile();

        static std::filesystem::path GetProcessPath();

        static void StartProcess();

        static bool IsLockFileDataValid(const LockFileData& LockData);

        LockFileData LockData;
    };
}