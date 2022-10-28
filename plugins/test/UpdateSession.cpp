#include "UpdateSession.h"

namespace L4::Plugin::Test
{
    UpdateSession::UpdateSession(Wrapper::Handle ClientHandle, const Wrapper::AppIdentity& OldIdentity, Wrapper::AppIdentity& NewIdentity) :
        Wrapper::IUpdateSession(ClientHandle)
    {
        NewIdentity = OldIdentity;
    }

    void UpdateSession::Start(Wrapper::Archive Archive, std::chrono::milliseconds ProgressUpdateRate)
    {
    }

    void UpdateSession::Pause()
    {
    }

    void UpdateSession::Resume()
    {
    }
}