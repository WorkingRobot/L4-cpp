#pragma once

#include <wrapper/IUpdate.h>

namespace L4::Plugin::FFXIV
{
    class Update final : public Wrapper::IUpdate
    {
    public:
        Update() = default;

        std::unique_ptr<Wrapper::IUpdateSession> CreateSession(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity) final;
    };
}