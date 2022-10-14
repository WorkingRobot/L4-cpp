#pragma once

#include "IUpdateSession.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class IUpdate
    {
    public:
        IUpdate();

        virtual ~IUpdate() = default;

        virtual std::unique_ptr<IUpdateSession> CreateSession(Handle ClientHandle, const AppIdentity& OldIdentity, AppIdentity& NewIdentity) = 0;
    };
}