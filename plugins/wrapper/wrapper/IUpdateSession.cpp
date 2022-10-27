#include "IUpdateSession.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    IUpdateSession::IUpdateSession(Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    Wrapper::UpdateState IUpdateSession::GetState() const
    {
        return IPlugin::Instance->Client.Interface.Update.GetState(ClientHandle);
    }

    void IUpdateSession::OnStart(const Wrapper::UpdateStartInfo& Info)
    {
        auto MarshalledInfo = To(Info);
        IPlugin::Instance->Client.Interface.Update.OnStart(ClientHandle, &MarshalledInfo);
    }

    void IUpdateSession::OnProgress(const Wrapper::UpdateProgressInfo& Info)
    {
        auto MarshalledInfo = To(Info);
        IPlugin::Instance->Client.Interface.Update.OnProgress(ClientHandle, &MarshalledInfo);
    }

    void IUpdateSession::OnPieceUpdate(uint64_t Id, Wrapper::UpdatePieceStatus Status)
    {
        IPlugin::Instance->Client.Interface.Update.OnPieceUpdate(ClientHandle, Id, Status);
    }

    void IUpdateSession::OnFinalize()
    {
        IPlugin::Instance->Client.Interface.Update.OnFinalize(ClientHandle);
    }

    void IUpdateSession::OnComplete()
    {
        IPlugin::Instance->Client.Interface.Update.OnComplete(ClientHandle);
    }
}