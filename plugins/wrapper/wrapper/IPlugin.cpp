#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    IPlugin::IPlugin(const libL4::ClientInterface* ClientInterface, std::unique_ptr<IAuth>&& AuthInterface, std::unique_ptr<IUpdate>&& UpdateInterface) :
        Client(ClientInterface),
        Auth(std::move(AuthInterface)),
        Update(std::move(UpdateInterface))
    {
        RawOps::Auth::Interface = Auth.get();
        RawOps::Update::Interface = Update.get();
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