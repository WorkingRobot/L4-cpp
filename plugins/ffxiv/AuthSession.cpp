#include "AuthSession.h"

#include "web/Http.h"

#include <format/Format.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <wrapper/AuthForm.h>

namespace L4::Plugin::FFXIV
{
    AuthSession::AuthSession(Auth& AuthModule) :
        AuthModule(AuthModule),
        CurrentState(State::UserPass)
    {
        // TODO: run "HandleBootCheck" here
        // https://github.com/goatcorp/FFXIVQuickLauncher/blob/d238e2473a53151274729f8ae85704f8cbfbefa4/src/XIVLauncher/Windows/ViewModel/MainWindowViewModel.cs#L1204
    }

    std::vector<Wrapper::AuthField> AuthSession::GetFields()
    {
        Wrapper::AuthForm Form;
        switch (CurrentState)
        {
        case State::UserPass:
            Form.Add<Wrapper::AuthFieldType::Text>(u8"username", u8"Username");
            Form.Add<Wrapper::AuthFieldType::Password>(u8"password", u8"Password");
            Form.Add<Wrapper::AuthFieldType::Checkbox>(u8"use-otp", u8"Use One-Time-Passwords");
            Form.Add<Wrapper::AuthFieldType::Checkbox>(u8"use-steam", u8"Use Steam service account");
            Form.Add<Wrapper::AuthFieldType::SubmitButton>(u8"submit", u8"Login");
            break;
        case State::OTP:
            Form.Add<Wrapper::AuthFieldType::Label>(u8"title", u8"Please enter your OTP key.");
            Form.Add<Wrapper::AuthFieldType::Text>(u8"otp", u8"OTP");
            Form.Add<Wrapper::AuthFieldType::SubmitButton>(u8"submit", u8"Continue");
            break;
        case State::Done:
            break;
        }
        return Form.Build();
    }

    Wrapper::AuthSubmitResponse AuthSession::Submit(const std::vector<Wrapper::AuthFulfilledField>& Fields)
    {
        Wrapper::AuthFulfilledForm Form(Fields);
        switch (CurrentState)
        {
        case State::UserPass:
        {
            Username = Form.Get<Wrapper::AuthFieldType::Text>(u8"username");
            Password = Form.Get<Wrapper::AuthFieldType::Password>(u8"password");
            UseOTP = Form.Get<Wrapper::AuthFieldType::Checkbox>(u8"use-otp");
            UseSteam = Form.Get<Wrapper::AuthFieldType::Checkbox>(u8"use-steam");

            if (Username.empty())
            {
                return { false, u8"Please enter an username." };
            }

            if (Username.contains(u8'@'))
            {
                return { false, u8"Please enter your SE account name, not your email address." };
            }

            if (Password.empty())
            {
                return { false, u8"Please enter a password." };
            }

            if (UseOTP)
            {
                CurrentState = State::OTP;
                return { true, u8"Please enter your OTP key." };
            }
            else
            {
                return SendToServer();
            }
        }
        case State::OTP:
        {
            OTP = Form.Get<Wrapper::AuthFieldType::Text>(u8"otp");

            if (OTP.size() != 6)
            {
                return { false, u8"Your key must be 6 digits long." };
            }

            return SendToServer();
        }
        case State::Done:
            return { true };
        }

        return { false, u8"Unknown state" };
    }

    Wrapper::AuthSubmitResponse AuthSession::SendToServer()
    {
        return { false, u8"Couldn't connect to server" };
    }
}