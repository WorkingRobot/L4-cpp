#pragma once

#include <wrapper/IAuth.h>

namespace L4::Plugin::FFXIV
{
    class Auth final : public Wrapper::IAuth
    {
    public:
        Auth() = default;

        std::optional<libL4::Marshal::UserIdentity> GetUser() final;

        std::unique_ptr<Wrapper::IAuthSession> CreateSession() final;
        
    private:
        friend class AuthSession;

        void SetIdentity(const libL4::Marshal::UserIdentity& User);

        std::optional<libL4::Marshal::UserIdentity> User;
    };
}