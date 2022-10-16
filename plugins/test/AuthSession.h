#pragma once

#include <wrapper/IAuthSession.h>

namespace L4::Plugin::Test
{
    class AuthSession final : public Wrapper::IAuthSession
    {
    public:
        AuthSession();

        std::vector<Wrapper::AuthField> GetFields() final;
        Wrapper::AuthSubmitResponse Submit(const std::vector<Wrapper::AuthFulfilledField>& Fields) final;

    private:
        enum class State : uint8_t
        {
            Initial,
            SignUp,
            Login,
            WebLogin,
            WebLoginCode,
            Done
        };

        State CurrentState;
    };
}