#include "Auth.h"

#include "AuthSession.h"

namespace L4::Plugin::Test
{
    std::optional<libL4::Marshal::UserIdentity> Auth::GetUser()
    {
        return User;
    }

    std::unique_ptr<Wrapper::IAuthSession> Auth::CreateSession()
    {
        return std::make_unique<AuthSession>(*this);
    }

    void Auth::SetIdentity(const libL4::Marshal::UserIdentity& User)
    {
        this->User.emplace(User);
    }
}