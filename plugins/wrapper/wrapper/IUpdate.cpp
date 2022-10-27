#include "IUpdate.h"

namespace L4::Plugin::Wrapper
{
    bool IUpdate::CreateSessionInternal(Handle ClientHandle, const AppIdentity& OldIdentity, AppIdentity& NewIdentity)
    {
        if (Sessions.find(ClientHandle) == Sessions.end())
        {
            Sessions.emplace(ClientHandle, CreateSession(ClientHandle, OldIdentity, NewIdentity));
            return true;
        }
        return false;
    }

    bool IUpdate::CloseSessionInternal(Handle ClientHandle)
    {
        return Sessions.erase(ClientHandle) == 1;
    }

    IUpdateSession* IUpdate::GetSessionInternal(Handle ClientHandle)
    {
        auto Itr = Sessions.find(ClientHandle);
        if (Itr != Sessions.end())
        {
            return Itr->second.get();
        }
        return nullptr;
    }
}