#include "AuthSession.h"

#include <wrapper/AuthForm.h>

namespace L4::Plugin::Test
{
    AuthSession::AuthSession() :
        CurrentState(State::Initial)
    {
    }

    std::vector<Wrapper::AuthField> AuthSession::GetFields()
    {
        Wrapper::AuthForm Ret;
        switch (CurrentState)
        {
        case State::Initial:
            Ret.Add<Wrapper::AuthFieldType::Text>(u8"username", u8"Username");
            break;
        }
        return Ret.Build();
    }

    Wrapper::AuthSubmitResponse AuthSession::Submit(const std::vector<Wrapper::AuthFulfilledField>& Fields)
    {
        return { true };
    }
}