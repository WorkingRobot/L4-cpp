#pragma once

#include "Auth.h"

#include <wrapper/IAuthSession.h>

namespace L4::Plugin::Test
{
    class AuthSession final : public Wrapper::IAuthSession
    {
    public:
        AuthSession(Auth& AuthModule);

        std::vector<libL4::Marshal::AuthField> GetFields() final;
        libL4::Marshal::AuthSubmitResponse Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields) final;

    private:
        enum class State : uint8_t
        {
            Initial,
            SignUp,
            SignUpCode,
            Login,
            OAuth,
            Import,
            Done
        };

        State CurrentState;

        struct SignUpData
        {
            std::u8string Username;
            std::u8string Email;
            std::u8string Password;
        };

        Auth& AuthModule;
        std::variant<std::monostate, SignUpData> Data;
    };
}