#pragma once

#include <optional>
#include <string>

namespace L4::Riot
{
    class ClientServices
    {
    public:
        ClientServices();

        bool IsRunning();

    private:
        struct LockFileData
        {
            std::string Name;
            uint32_t Pid;
            uint16_t Port;
            std::string PrivateToken;
            std::string Scheme;
        };

        static std::optional<LockFileData> GetLockFileData();
    };
}