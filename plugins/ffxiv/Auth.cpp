#include "Auth.h"

namespace L4::Plugin::FFXIV
{
    std::optional<libL4::Marshal::UserIdentity> Auth::GetUser()
    {
        return User;
    }

    std::unique_ptr<Wrapper::IAuthSession> Auth::CreateSession()
    {
        return nullptr;
        //std::make_unique<AuthSession>(*this);
    }

    void Auth::SetIdentity(const libL4::Marshal::UserIdentity& User)
    {
        this->User.emplace(User);
    }
}