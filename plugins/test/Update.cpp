#include "Update.h"

#include "UpdateSession.h"

namespace L4::Plugin::Test
{
    std::unique_ptr<Wrapper::IUpdateSession> Update::CreateSession(Wrapper::Handle ClientHandle, const Wrapper::AppIdentity& OldIdentity, Wrapper::AppIdentity& NewIdentity)
    {
        return std::make_unique<UpdateSession>(ClientHandle, OldIdentity, NewIdentity);
    }
}