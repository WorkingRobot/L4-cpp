#pragma once

#include "IAuthSession.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <memory>
#include <unordered_map>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class IAuth
    {
    public:
        IAuth() = default;

        virtual ~IAuth() = default;

        void OnUserUpdated();

        virtual std::optional<UserIdentity> GetUser() = 0;

        virtual std::unique_ptr<IAuthSession> CreateSession() = 0;

    private:
        friend class IPlugin;
        bool CreateSessionInternal(Handle ClientHandle);
        bool CloseSessionInternal(Handle ClientHandle);
        IAuthSession* GetSessionInternal(Handle ClientHandle);

        std::unordered_map<Handle, std::unique_ptr<IAuthSession>> Sessions;
    };
}