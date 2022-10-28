#pragma once

#include <wrapper/IAuth.h>

namespace L4::Plugin::Test
{
    class Auth final : public Wrapper::IAuth
    {
    public:
        Auth() = default;

        std::optional<Wrapper::UserIdentity> GetUser() final;

        std::unique_ptr<Wrapper::IAuthSession> CreateSession() final;

    private:
        friend class AuthSession;

        void SetIdentity(const Wrapper::UserIdentity& User);

        std::optional<Wrapper::UserIdentity> User;
    };
}