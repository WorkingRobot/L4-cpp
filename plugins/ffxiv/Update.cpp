#include "Update.h"

namespace L4::Plugin::FFXIV
{
    std::unique_ptr<Wrapper::IUpdateSession> Update::CreateSession(Wrapper::Handle ClientHandle, const Wrapper::AppIdentity& OldIdentity, Wrapper::AppIdentity& NewIdentity)
    {
        return nullptr;
    }
}