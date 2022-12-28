#include "Update.h"

#include "UpdateSession.h"

namespace L4::Plugin::Test
{
    std::unique_ptr<Wrapper::IUpdateSession> Update::CreateSession(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity)
    {
        return std::make_unique<UpdateSession>(ClientHandle, OldIdentity, NewIdentity);
    }
}