#include "IUpdateSession.h"

#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    IUpdateSession::IUpdateSession(libL4::Handle ClientHandle) :
        ClientHandle(ClientHandle)
    {
    }

    libL4::UpdateState IUpdateSession::GetState() const
    {
        return IPlugin::Instance->Client->Update.GetState(ClientHandle);
    }

    void IUpdateSession::OnStart(const libL4::Marshal::UpdateStartInfo& Info)
    {
        auto MarshalledInfo = To(Info);
        IPlugin::Instance->Client->Update.OnStart(ClientHandle, &MarshalledInfo);
    }

    void IUpdateSession::OnProgress(const libL4::Marshal::UpdateProgressInfo& Info)
    {
        auto MarshalledInfo = To(Info);
        IPlugin::Instance->Client->Update.OnProgress(ClientHandle, &MarshalledInfo);
    }

    void IUpdateSession::OnPieceUpdate(uint64_t Id, libL4::UpdatePieceStatus Status)
    {
        IPlugin::Instance->Client->Update.OnPieceUpdate(ClientHandle, Id, Status);
    }

    void IUpdateSession::OnFinalize()
    {
        IPlugin::Instance->Client->Update.OnFinalize(ClientHandle);
    }

    void IUpdateSession::OnComplete()
    {
        IPlugin::Instance->Client->Update.OnComplete(ClientHandle);
    }
}