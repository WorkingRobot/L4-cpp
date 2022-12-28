#include "IAuth.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    void IAuth::OnUserUpdated()
    {
        IPlugin::Instance->Client->Auth.OnUserUpdated(IPlugin::Instance->ClientHandle);
    }

    bool IAuth::CreateSessionInternal(libL4::Handle ClientHandle)
    {
        if (Sessions.find(ClientHandle) == Sessions.end())
        {
            Sessions.emplace(ClientHandle, CreateSession());
            return true;
        }
        return false;
    }

    bool IAuth::CloseSessionInternal(libL4::Handle ClientHandle)
    {
        return Sessions.erase(ClientHandle) == 1;
    }

    IAuthSession* IAuth::GetSessionInternal(libL4::Handle ClientHandle)
    {
        auto Itr = Sessions.find(ClientHandle);
        if (Itr != Sessions.end())
        {
            return Itr->second.get();
        }
        return nullptr;
    }
}