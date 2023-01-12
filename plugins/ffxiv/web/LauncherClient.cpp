#include "LauncherClient.h"

#include "SteamClient.h"

#include <openssl/sha.h>
#include <utils/Crc32.h>
#include <web/Http.h>

namespace L4::Plugin::FFXIV
{
    using Web::Http::Response;

    std::u16string GetComputerIdHashString();

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/4aeb71f80df7cbe5dcdef0080fb26eceeff208f5/src/XIVLauncher.Common/Game/Launcher.cs#L636
    static std::string GenerateComputerId()
    {
        uint8_t Hash[20] {};

        std::u16string HashString = GetComputerIdHashString();
        SHA1((uint8_t*)HashString.c_str(), HashString.size() * sizeof(char16_t), Hash);

        uint8_t Checksum[5] { uint8_t(-(Hash[0] + Hash[1] + Hash[2] + Hash[3])), Hash[0], Hash[1], Hash[2], Hash[3] };

        return fmt::format("{:02x}", fmt::join(std::as_bytes(std::span(Checksum)), ""));
    }

    static std::string GetUnixMsTimestamp()
    {
        return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    static std::string GetFrontierReferer()
    {
        return fmt::format("https://launcher.finalfantasyxiv.com/v620/index.html?rc_lang={:s}&time={:%F-%H-%M}", "en_us", std::chrono::system_clock::now());
    }

    LauncherClient::LauncherClient() :
        UserAgent(fmt::format("SQEXAuthor/2.0.0(Windows 6.2; {:s}; {:s})", "ja-jp", GenerateComputerId())),
        FrontierHeaders {
            { "Accept-Encoding", "gzip, deflate" },
            // vvvv https://github.com/goatcorp/FFXIVQuickLauncher/blob/9409ed827b087d5b86aad2293eab74c6fe4f2214/src/XIVLauncher.Common/Util/ApiHelpers.cs#L29
            { "Accept-Language", "en-us" },
            // ^^^^ Overengineered and honestly it just looks like goat was bored
            { "Origin", "https://launcher.finalfantasyxiv.com" },
            { "Referer", GetFrontierReferer() }
        },
#ifdef CONFIG_VERSION_PLATFORM_osx
        PatcherUserAgent("FFXIV-MAC PATCH CLIENT")
#else
        PatcherUserAgent("FFXIV PATCH CLIENT")
#endif
    {
    }

    Response<bool> LauncherClient::GetLoginStatus()
    {
        return Web::Http::Get<Models::GateStatus>(
                   cpr::Url("https://frontier.ffxiv.com/worldStatus/login_status.json"),
                   cpr::Parameters {
                       { "_", GetUnixMsTimestamp() } },
                   UserAgent,
                   FrontierHeaders)
            .transform(&Models::GateStatus::Status);
    }

    Response<Models::GateStatus> LauncherClient::GetGateStatus()
    {
        return Web::Http::Get<Models::GateStatus>(
            cpr::Url("https://frontier.ffxiv.com/worldStatus/gate_status.json"),
            cpr::Parameters {
                { "lang", "en-us" },
                { "_", GetUnixMsTimestamp() } },
            UserAgent,
            FrontierHeaders);
    }

    Response<Models::PatchListBoot> LauncherClient::GetBootPatchList(const std::string& Version)
    {
        return Web::Http::Get<Models::PatchListBoot>(
            cpr::Url(fmt::format("http://patch-bootver.ffxiv.com/http/win32/ffxivneo_release_boot/{:s}/", Version)),
            cpr::Parameters {
                { "time", fmt::format("{:%F-%H-%M}", std::chrono::floor<std::chrono::duration<int, std::ratio<60 * 10>>>(std::chrono::system_clock::now())) } },
            PatcherUserAgent);
    }

    Response<Models::PatchListGame> LauncherClient::GetGamePatchList(const std::string& Version, const std::string& SessionId, const Models::VersionReport& VersionReport)
    {
        return Web::Http::Post<Models::PatchListGame>(
            cpr::Url(fmt::format("https://patch-gamever.ffxiv.com/http/win32/ffxivneo_release_game/{:s}/{:s}", Version, SessionId)),
            cpr::Body { VersionReport.Serialize() },
            cpr::Header {
                { "X-Hash-Check", "enabled" } },
            PatcherUserAgent);
    }

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher.Common/Game/Launcher.cs#L112
    Response<Models::LoginResult> LauncherClient::Login(const std::string& Username, const std::string& Password, const std::string& Otp, std::optional<Models::SteamTicket> SteamTicket, bool IsFreeTrial)
    {
        return GetLoginToken(SteamTicket, IsFreeTrial).and_then([&, this](const LoginToken& Token) { return LoginOAuth(Token, Username, Password, Otp); });
    }

    static std::string_view RetrieveToken(std::string_view View, const std::string_view Begin, const std::string_view End)
    {
        auto BeginPos = View.find(Begin);
        if (BeginPos == View.npos)
        {
            return {};
        }

        View = View.substr(BeginPos + Begin.size());

        auto EndPos = View.find(End);
        if (EndPos == View.npos)
        {
            return {};
        }

        return View.substr(0, EndPos);
    };

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher.Common/Game/Launcher.cs#L453
    Response<LauncherClient::LoginToken> LauncherClient::GetLoginToken(std::optional<Models::SteamTicket> SteamTicket, bool IsFreeTrial)
    {
        std::string LoginTokenUrl;

        {
            cpr::Parameters Params {
                { "lng", "en" },
                { "rgn", "3" },
                { "isft", IsFreeTrial ? "1" : "0" },
                { "cssmode", "1" },
                { "isnew", "1" },
                { "launchver", "3" },
            };
            if (SteamTicket)
            {
                Params.Add({ "issteam", "1" });
                Params.Add({ "session_ticket", SteamTicket->Text });
                Params.Add({ "ticket_size", std::to_string(SteamTicket->Length) });
            }

            LoginTokenUrl = fmt::format("https://ffxiv-login.square-enix.com/oauth/ffxivarr/login/top?{:s}", Params.GetContent({}));
        }

        return Web::Http::Get<std::string>(
                   cpr::Url(LoginTokenUrl),
                   FrontierHeaders,
                   cpr::Header {
                       { "Accept", "image/gif, image/jpeg, image/pjpeg, application/x-ms-application, application/xaml+xml, application/x-ms-xbap, */*" },
                       { "Cookie", "_rsid=\"\"" } },
                   UserAgent)
            .and_then([&](std::string_view Resp) -> Response<LauncherClient::LoginToken> {
                if (Resp.contains("window.external.user(\"restartup\");"))
                {
                    return std::unexpected(Web::Http::Error { Web::Http::ErrorType::BadFormat, "No Steam account is linked." });
                }

                LauncherClient::LoginToken Token;
                Token.RefererUrl = LoginTokenUrl;

                {
                    std::string_view StoredToken = RetrieveToken(Resp, "name=\"_STORED_\" value=\"", "\"");
                    if (StoredToken.empty())
                    {
                        return std::unexpected(Web::Http::Error { Web::Http::ErrorType::BadFormat, "Could not get STORED token." });
                    }
                    Token.StoredToken = StoredToken;
                }

                if (SteamTicket)
                {
                    std::string_view SteamUsername = RetrieveToken(Resp, "<input name=\"sqexid\" type=\"hidden\" value=\"", "\"");
                    if (SteamUsername.empty())
                    {
                        return std::unexpected(Web::Http::Error { Web::Http::ErrorType::BadFormat, "Could not get Steam username." });
                    }
                    Token.SteamUsername = SteamUsername;
                }

                return Token;
            });
    }

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher.Common/Game/Launcher.cs#L538
    Response<Models::LoginResult> LauncherClient::LoginOAuth(const LoginToken& Token, const std::string& Username, const std::string& Password, const std::string& Otp)
    {
        if (Token.SteamUsername)
        {
            if (Username != Token.SteamUsername)
            {
                return std::unexpected(Web::Http::Error { Web::Http::ErrorType::BadHttpCode, fmt::format("This Steam account is linked to a different FFXIV account. Your steam account is linked to {:s}.", Token.SteamUsername.value()) });
            }
        }

        return Web::Http::Post<std::string>(
                   cpr::Url("https://ffxiv-login.square-enix.com/oauth/ffxivarr/login/login.send"),
                   FrontierHeaders,
                   cpr::Header {
                       { "Referer", Token.RefererUrl },
                       { "Accept", "image/gif, image/jpeg, image/pjpeg, application/x-ms-application, application/xaml+xml, application/x-ms-xbap, */*" },
                       { "Cookie", "_rsid=\"\"" } },
                   cpr::Payload {
                       { "_STORED_", Token.StoredToken },
                       { "sqexid", Username },
                       { "password", Password },
                       { "otppw", Otp },
                   },
                   UserAgent)
            .and_then([&](std::string_view Resp) -> Response<Models::LoginResult> {
                std::string_view LaunchParams = RetrieveToken(Resp, "window.external.user(\"login=auth,ok,", "\");");
                if (LaunchParams.empty())
                {
                    std::string_view ErrorMessage = RetrieveToken(Resp, "window.external.user(\"login=auth,ng,err,", "\");");
                    if (ErrorMessage.empty())
                    {
                        ErrorMessage = "Unknown login error";
                    }
                    return std::unexpected(Web::Http::Error { Web::Http::ErrorType::BadHttpCode, std::string(ErrorMessage) });
                }
                Models::LoginResult Result;
                std::ranges::for_each(LaunchParams | std::views::split(',') | std::views::chunk(2), [&](const auto& Entry) {
                    auto EItr = Entry.begin();
                    auto KRng = *EItr++;
                    auto VRng = *EItr++;
                    std::string_view K(KRng.begin(), KRng.end());
                    std::string_view V(VRng.begin(), VRng.end());

                    switch (Crc32(K))
                    {
                    case Crc32("sid"):
                        Result.SessionId = V;
                        break;
                    case Crc32("terms"):
                        Result.AcceptedToS = ParseNum<int>(V).value_or(0);
                        break;
                    case Crc32("region"):
                        Result.Region = ParseNum<int>(V).value_or(0);
                        break;
                    case Crc32("etmadd"):
                        break;
                    case Crc32("playable"):
                        Result.IsPlayable = ParseNum<int>(V).value_or(0);
                        break;
                    case Crc32("ps3pkg"):
                        break;
                    case Crc32("maxex"):
                        Result.MaxExpansion = ParseNum<int>(V).value_or(0);
                        break;
                    case Crc32("product"):
                        break;
                    }
                });
                return Result;
            });
    }
}