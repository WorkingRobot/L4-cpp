#pragma once

#include <wrapper/IAuth.h>

namespace L4::Plugin::Test
{
    class Auth final : public Wrapper::IAuth
    {
    public:
        Auth();

        std::optional<Wrapper::UserIdentity> GetUser() final;

        std::unique_ptr<Wrapper::IAuthSession> CreateSession() final;

    private:
        std::optional<Wrapper::UserIdentity> User;
    };
}