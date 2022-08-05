#pragma once

#include <source/Upstream.h>
#include <L4/web/Http.h>
#include <L4/web/Json.h>

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    struct UpstreamAppCtx
    {
        std::u8string InternalName;
    };

    class Upstream : public IUpstream
    {
        template <class T, class... ArgTs>
        friend OwningPtr<T> L4::Interface::CreateObject(ArgTs&&... Args);

    public:
        Upstream(IUpstream::FIPtr Interface);

        void Authenticate() final;

        Ptr<std::vector<UpstreamApp>> GetAvailableApps(bool ForceRefresh) final;

        const UpstreamApp* CheckForArchiveUpdate(Ptr<IArchive> Archive) final;

        OwningPtr<IUpdater> OpenUpdater(IUpdater::FIPtr Interface) const final;

    private:
        OwningPtr<std::vector<UpstreamApp>> AvailableApps;
        Web::Json::Document ClientConfig;
    };
}