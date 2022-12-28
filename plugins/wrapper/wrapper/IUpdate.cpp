#include "IUpdate.h"

namespace L4::Plugin::Wrapper
{
    bool IUpdate::CreateSessionInternal(libL4::Handle ClientHandle, const libL4::Marshal::AppIdentity& OldIdentity, libL4::Marshal::AppIdentity& NewIdentity)
    {
        if (Sessions.find(ClientHandle) == Sessions.end())
        {
            Sessions.emplace(ClientHandle, CreateSession(ClientHandle, OldIdentity, NewIdentity));
            return true;
        }
        return false;
    }

    bool IUpdate::CloseSessionInternal(libL4::Handle ClientHandle)
    {
        return Sessions.erase(ClientHandle) == 1;
    }

    IUpdateSession* IUpdate::GetSessionInternal(libL4::Handle ClientHandle)
    {
        auto Itr = Sessions.find(ClientHandle);
        if (Itr != Sessions.end())
        {
            return Itr->second.get();
        }
        return nullptr;
    }
}