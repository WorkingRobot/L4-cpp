#pragma once

#include "Update.h"

#include <wrapper/IUpdateSession.h>

namespace L4::Plugin::Test
{
    class UpdateSession final : public Wrapper::IUpdateSession
    {
    public:
        UpdateSession(Wrapper::Handle ClientHandle, const Wrapper::AppIdentity& OldIdentity, Wrapper::AppIdentity& NewIdentity);

        void Start(Wrapper::Archive Archive, std::chrono::milliseconds ProgressUpdateRate) final;

        void Pause() final;

        void Resume() final;
    };
}