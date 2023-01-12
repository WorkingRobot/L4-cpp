#include "AuthSession.h"

#include <format/Format.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <wrapper/AuthForm.h>

namespace L4::Plugin::Test
{
    AuthSession::AuthSession(Auth& AuthModule) :
        AuthModule(AuthModule),
        CurrentState(State::Initial)
    {
    }

    std::vector<libL4::Marshal::AuthField> AuthSession::GetFields()
    {
        Wrapper::AuthForm Form;
        switch (CurrentState)
        {
        case State::Initial:
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"sign-up", u8"Sign Up");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"login", u8"Login");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"oauth", u8"Open in Browser");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"import", u8"Import Credentials");
            break;
        case State::SignUp:
            Form.Add<libL4::AuthFieldType::Text>(u8"username", u8"Username");
            Form.Add<libL4::AuthFieldType::Text>(u8"email", u8"Email");
            Form.Add<libL4::AuthFieldType::Password>(u8"password", u8"Password");
            Form.Add<libL4::AuthFieldType::Password>(u8"password-repeat", u8"Repeat Password");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"submit", u8"Sign Up");
            break;
        case State::SignUpCode:
            Form.Add<libL4::AuthFieldType::Text>(u8"code", u8"Code");
            Form.Add<libL4::AuthFieldType::Checkbox>(u8"remember-me", u8"Remember Me");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"submit", u8"Sign Up");
            break;
        case State::Login:
            Form.Add<libL4::AuthFieldType::Text>(u8"username", u8"Username");
            Form.Add<libL4::AuthFieldType::Password>(u8"password", u8"Password");
            Form.Add<libL4::AuthFieldType::Checkbox>(u8"remember-me", u8"Remember Me");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"submit", u8"Login");
            break;
        case State::OAuth:
            Form.Add<libL4::AuthFieldType::Checkbox>(u8"remember-me", u8"Remember Me");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"open-browser", u8"Open in Browser");
            Form.Add<libL4::AuthFieldType::Label>(u8"submit-description", u8"Click Done after you've logged in through the browser");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"submit", u8"Done");
            break;
        case State::Import:
            Form.Add<libL4::AuthFieldType::Text>(u8"data", u8"Data");
            Form.Add<libL4::AuthFieldType::Checkbox>(u8"remember-me", u8"Remember Me");
            Form.Add<libL4::AuthFieldType::SubmitButton>(u8"submit", u8"Login");
            break;
        case State::Done:
            break;
        }
        return Form.Build();
    }

    libL4::Marshal::AuthSubmitResponse AuthSession::Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields)
    {
        Wrapper::AuthFulfilledForm Form(Fields);
        switch (CurrentState)
        {
        case State::Initial:
        {
            if (Form.Get<libL4::AuthFieldType::SubmitButton>(u8"sign-up"))
            {
                CurrentState = State::SignUp;
                Data.emplace<SignUpData>();
            }
            else if (Form.Get<libL4::AuthFieldType::SubmitButton>(u8"login"))
            {
                CurrentState = State::Login;
            }
            else if (Form.Get<libL4::AuthFieldType::SubmitButton>(u8"oauth"))
            {
                CurrentState = State::OAuth;
            }
            else if (Form.Get<libL4::AuthFieldType::SubmitButton>(u8"import"))
            {
                CurrentState = State::Import;
            }
            else
            {
                return { false, u8"No button was pressed?" };
            }
            return { true };
        }
        case State::SignUp:
        {
            auto& Data = std::get<SignUpData>(this->Data);
            Data.Username = Form.Get<libL4::AuthFieldType::Text>(u8"username");
            Data.Email = Form.Get<libL4::AuthFieldType::Text>(u8"email");
            Data.Password = Form.Get<libL4::AuthFieldType::Password>(u8"password");

            static constexpr auto IsAlphaL = [](char8_t Char) {
                return u8'a' <= Char && Char <= u8'z';
            };

            static constexpr auto IsAlphaU = [](char8_t Char) {
                return u8'A' <= Char && Char <= u8'Z';
            };

            static constexpr auto IsDigit = [](char8_t Char) {
                return u8'0' <= Char && Char <= u8'9';
            };

            static constexpr auto IsPunctuation = [](char8_t Char) {
                return std::u8string_view(u8"!@#$%^&*").find(Char) != std::u8string_view::npos;
            };

            static constexpr auto IsAlphaNum = [](char8_t Char) {
                return IsAlphaL(Char) || IsAlphaU(Char) || IsDigit(Char);
            };

            static constexpr auto IsEmailChar = [](char8_t Char) {
                return IsAlphaNum(Char) || Char == u8'+' || Char == u8'-' || Char == u8'.' || Char == u8'_';
            };

            static constexpr auto IsValidEmail = [](std::u8string_view Input) {
                auto At = Input.find(u8'@');
                if (At == Input.npos)
                {
                    return false;
                }
                auto Recipient = Input.substr(0, At);
                auto Domain = Input.substr(At + 1);
                if (Domain.find(u8'.') == Domain.npos)
                {
                    return false;
                }
                return std::ranges::all_of(Recipient, IsEmailChar) && std::ranges::all_of(Domain, IsEmailChar);
            };

            if (!IsValidEmail(Data.Email))
            {
                return { false, u8"Email is invalid" };
            }

            static constexpr auto IsUsernameChar = [](char8_t Char) {
                return IsAlphaNum(Char) || Char == u8'-' || Char == u8'.' || Char == u8'_';
            };

            static constexpr auto IsValidUsername = [](std::u8string_view Input) {
                if (Input.size() < 3 || Input.size() > 20)
                {
                    return false;
                }
                return std::ranges::all_of(Input, IsUsernameChar);
            };

            if (!IsValidEmail(Data.Username))
            {
                return { false, u8"Username is invalid" };
            }

            static constexpr auto IsValidPassword = [](std::u8string_view Input) {
                return !(Input.size() < 8 ||
                         std::ranges::none_of(Input, IsAlphaL) ||
                         std::ranges::none_of(Input, IsAlphaU) ||
                         std::ranges::none_of(Input, IsDigit) ||
                         std::ranges::none_of(Input, IsPunctuation));
            };

            if (!IsValidPassword(Data.Password))
            {
                return { false, u8"Password is invalid" };
            }

            if (Data.Password != Form.Get<libL4::AuthFieldType::Password>(u8"password-repeat"))
            {
                return { false, u8"Passwords don't match" };
            }

            CurrentState = State::SignUpCode;
            return { true, fmt::format(u8"A confirmation code was sent to {:s}", Data.Email) };
        }
        case State::SignUpCode:
        {
            auto& Data = std::get<SignUpData>(this->Data);
            auto Code = Form.Get<libL4::AuthFieldType::Text>(u8"code");

            uint8_t Digest[20];
            {
                auto Ctx = EVP_MD_CTX_create();
                EVP_DigestInit_ex(Ctx, EVP_sha1(), NULL);
                EVP_DigestUpdate(Ctx, Data.Username.c_str(), Data.Username.size());
                EVP_DigestUpdate(Ctx, "Don'tMessWithMMS", 16);
                EVP_DigestUpdate(Ctx, Data.Email.c_str(), Data.Email.size());
                EVP_DigestFinal_ex(Ctx, Digest, NULL);
                EVP_MD_CTX_destroy(Ctx);
            }

            auto Timestep = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            Timestep /= 30;
            if constexpr (std::endian::native == std::endian::little)
            {
                Timestep = std::byteswap(Timestep);
            }

            uint8_t Result[20];
            HMAC(EVP_sha1(), Digest, sizeof(Digest), (uint8_t*)&Timestep, sizeof(Timestep), Result, NULL);
            auto Offset = Result[19] & 0x0f;
            auto BinCode = (Result[Offset] & 0x7f) << 24 |
                           (Result[Offset + 1] & 0xff) << 16 |
                           (Result[Offset + 2] & 0xff) << 8 |
                           (Result[Offset + 3] & 0xff);

            auto Totp = BinCode % 1000000;

            auto TotpString = std::to_string(Totp);
            if (std::u8string_view((char8_t*)TotpString.c_str(), TotpString.size()) != Code)
            {
                return { false, fmt::format(u8"Invalid code. Should be {:d}", Totp) };
            }

            AuthModule.SetIdentity({ .Id = Data.Username, .Name = Data.Username });

            CurrentState = State::Done;
            return { true };
        }
        case State::Login:
        {
            auto Username = Form.Get<libL4::AuthFieldType::Text>(u8"username");

            AuthModule.SetIdentity({ .Id = Username, .Name = Username });

            CurrentState = State::Done;
            return { true };
        }
        case State::OAuth:
        {
            auto ShouldOpenBrowser = Form.Get<libL4::AuthFieldType::SubmitButton>(u8"open-browser");
            if (ShouldOpenBrowser)
            {
                auto RememberMe = Form.Get<libL4::AuthFieldType::Checkbox>(u8"remember-me");

                // Just a dummy function
                auto LaunchBrowser = [](const std::u8string& Url) {};

                // Set up the browser callback here

                // Either use a custom protocol (i.e. riotclient://...) which launches a separate application that
                // communicates back here or set up a local webserver on a unique port that the browser will redirect to.

                LaunchBrowser(fmt::format(u8"https://auth.camora.dev/oauth{:s}", RememberMe ? u8"?remember_me" : u8""));

                return { false };
            }
            else
            {
                // Check if callback data is recieved here

                // Dummy data
                AuthModule.SetIdentity({ .Id = u8"e3f45f056574e805245c18fbbf297d60", .Name = u8"Asriel" });

                CurrentState = State::Done;
                return { true };
            }
        }
        case State::Import:
        {
            auto Data = Form.Get<libL4::AuthFieldType::Text>(u8"data");
            std::u8string_view DataView = Data;

            auto Sep = DataView.find(u8';');
            if (Sep == DataView.npos)
            {
                return { false, u8"Invalid login data" };
            }
            auto Id = DataView.substr(0, Sep);
            auto Name = DataView.substr(Sep + 1);

            AuthModule.SetIdentity({ .Id = std::u8string(Id), .Name = std::u8string(Name) });

            CurrentState = State::Done;
            return { true };
        }
        case State::Done:
            return { true };
        }

        return { false, u8"Unknown state" };
    }
}