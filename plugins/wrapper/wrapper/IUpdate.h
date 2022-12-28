#pragma once

#include "IUpdateSession.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>
#include <unordered_map>

namespace L4::Plugin::Wrapper
{
    class IUpdate
    {
    public:
        IUpdate() = default;

        virtual ~IUpdate() = default;

        virtual std::unique_ptr<IUpdateSession> CreateSession(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity) = 0;

    private:
        friend class IPlugin;
        bool CreateSessionInternal(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity);
        bool CloseSessionInternal(libL4::Handle ClientHandle);
        IUpdateSession* GetSessionInternal(libL4::Handle ClientHandle);

        std::unordered_map<libL4::Handle, std::unique_ptr<IUpdateSession>> Sessions;
    };
}