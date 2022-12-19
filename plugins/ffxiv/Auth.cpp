#include "Auth.h"

namespace L4::Plugin::FFXIV
{
    std::optional<Wrapper::UserIdentity> Auth::GetUser()
    {
        return User;
    }

    std::unique_ptr<Wrapper::IAuthSession> Auth::CreateSession()
    {
        return nullptr;
        //std::make_unique<AuthSession>(*this);
    }

    void Auth::SetIdentity(const Wrapper::UserIdentity& User)
    {
        this->User.emplace(User);
    }
}