#pragma once

#include <wrapper/IUpdate.h>

namespace L4::Plugin::Test
{
    class Update final : public Wrapper::IUpdate
    {
    public:
        Update();

        std::unique_ptr<Wrapper::IUpdateSession> CreateSession(Wrapper::Handle ClientHandle, const Wrapper::AppIdentity& OldIdentity, Wrapper::AppIdentity& NewIdentity) final;
    };
}