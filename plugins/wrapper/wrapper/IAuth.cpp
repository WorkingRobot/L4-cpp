#include "IAuth.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    void IAuth::OnUserUpdated()
    {
        IPlugin::Instance->Client.Interface.Auth.OnUserUpdated(IPlugin::Instance->Client.Interface.PluginHandle);
    }

    bool IAuth::CreateSessionInternal(Handle ClientHandle)
    {
        if (Sessions.find(ClientHandle) == Sessions.end())
        {
            Sessions.emplace(ClientHandle, CreateSession());
            return true;
        }
        return false;
    }

    bool IAuth::CloseSessionInternal(Handle ClientHandle)
    {
        return Sessions.erase(ClientHandle) == 1;
    }

    IAuthSession* IAuth::GetSessionInternal(Handle ClientHandle)
    {
        auto Itr = Sessions.find(ClientHandle);
        if (Itr != Sessions.end())
        {
            return Itr->second.get();
        }
        return nullptr;
    }
}