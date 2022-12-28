#pragma once

#include "Auth.h"

#include <wrapper/IAuthSession.h>

namespace L4::Plugin::FFXIV
{
    // Based off of https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher/Windows/ViewModel/MainWindowViewModel.cs#L162
    class AuthSession final : public Wrapper::IAuthSession
    {
    public:
        AuthSession(Auth& AuthModule);

        std::vector<libL4::Marshal::AuthField> GetFields() final;
        libL4::Marshal::AuthSubmitResponse Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields) final;

    private:
        libL4::Marshal::AuthSubmitResponse SendToServer();

        enum class State : uint8_t
        {
            UserPass,
            OTP,
            Done
        };

        State CurrentState;

        std::u8string Username;
        std::u8string Password;
        bool UseOTP;
        bool UseSteam;
        std::u8string OTP;

        Auth& AuthModule;
    };
}