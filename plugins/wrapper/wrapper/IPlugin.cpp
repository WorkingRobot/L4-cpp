#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    IPlugin::IPlugin(const libL4::ClientInterface* ClientInterface) :
        Client(ClientInterface)
    {
    }

    libL4::PluginInterface IPlugin::GetRawInterface()
    {
        return {
            .Identity = To(GetIdentity()),
            .Auth = {
                .GetUser = RawOps::Auth::GetUser,
                .Open = RawOps::Auth::Open,
                .Close = RawOps::Auth::Close,
                .GetFields = RawOps::Auth::GetFields,
                .Submit = RawOps::Auth::Submit,
            },
            .Update = {
                .Open = RawOps::Update::Open,
                .Close = RawOps::Update::Close,
                .Start = RawOps::Update::Start,
                .Pause = RawOps::Update::Pause,
                .Resume = RawOps::Update::Resume,
            }
        };
    }
}