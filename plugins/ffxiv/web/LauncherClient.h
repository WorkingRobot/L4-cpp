#pragma once

#include "models/GateStatus.h"
#include "models/LoginResult.h"
#include "models/PatchList.h"
#include "models/SteamTicket.h"
#include "models/VersionReport.h"

#include <web/Client.h>

namespace L4::Plugin::FFXIV
{
    class LauncherClient : public Web::Http::Client
    {
    public:
        LauncherClient();

        Response<bool> GetLoginStatus();

        Response<Models::GateStatus> GetGateStatus();

        Response<Models::PatchListBoot> GetBootPatchList(const std::string& Version);

        // Patcher function: https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher/Windows/ViewModel/MainWindowViewModel.cs#L1247
        
        Response<Models::PatchListGame> GetGamePatchList(const std::string& Version, const std::string& SessionId, const Models::VersionReport& VersionReport);

        Response<Models::LoginResult> Login(const std::string& Username, const std::string& Password, const std::string& Otp, std::optional<Models::SteamTicket> SteamTicket, bool IsFreeTrial);

        static constexpr std::string_view BaseGameVersion = "2012.01.01.0000.0000";

    private:
        struct LoginToken
        {
            std::string RefererUrl;
            std::string StoredToken;
            std::optional<std::string> SteamUsername;
        };

        Response<LoginToken> GetLoginToken(std::optional<Models::SteamTicket> SteamTicket, bool IsFreeTrial);

        Response<Models::LoginResult> LoginOAuth(const LoginToken& Token, const std::string& Username, const std::string& Password, const std::string& Otp);

        cpr::UserAgent UserAgent;
        cpr::Header FrontierHeaders;

        cpr::UserAgent PatcherUserAgent;
    };
}