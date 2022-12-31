#pragma once

#include <libL4/marshal/Marshal.h>

namespace L4::Manager
{
    class AuthSession
    {
    public:
        AuthSession(libL4::AuthCallbacks Callbacks);

        ~AuthSession();

        [[nodiscard]] std::vector<libL4::Marshal::AuthField> GetFields();

        libL4::Marshal::AuthSubmitResponse Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields);

    private:
        libL4::AuthCallbacks Callbacks;
    };
}