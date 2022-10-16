#include "AuthSession.h"

namespace L4::Plugin::Test
{
    AuthSession::AuthSession() :
        CurrentState(State::Initial)
    {
    }

    std::vector<Wrapper::AuthField> AuthSession::GetFields()
    {
        switch (CurrentState)
        {
        }
        return {};
    }

    Wrapper::AuthSubmitResponse AuthSession::Submit(const std::vector<Wrapper::AuthFulfilledField>& Fields)
    {
        return { true };
    }
}