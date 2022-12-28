#pragma once

#include "IAuthSession.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <memory>
#include <unordered_map>

namespace L4::Plugin::Wrapper
{
    class IAuth
    {
    public:
        IAuth() = default;

        virtual ~IAuth() = default;

        void OnUserUpdated();

        virtual std::optional<libL4::Marshal::UserIdentity> GetUser() = 0;

        virtual std::unique_ptr<IAuthSession> CreateSession() = 0;

    private:
        friend class IPlugin;
        bool CreateSessionInternal(libL4::Handle ClientHandle);
        bool CloseSessionInternal(libL4::Handle ClientHandle);
        IAuthSession* GetSessionInternal(libL4::Handle ClientHandle);

        std::unordered_map<libL4::Handle, std::unique_ptr<IAuthSession>> Sessions;
    };
}