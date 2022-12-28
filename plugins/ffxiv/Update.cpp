#include "Update.h"

namespace L4::Plugin::FFXIV
{
    std::unique_ptr<Wrapper::IUpdateSession> Update::CreateSession(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity)
    {
        return nullptr;
    }
}