#include "UpdateSession.h"

namespace L4::Plugin::Test
{
    UpdateSession::UpdateSession(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity) :
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