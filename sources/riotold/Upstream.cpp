#include "Upstream.h"

#include "json/Patchline.h"
//#include "UpstreamApp.h"

#include <source/Error.h>

namespace L4::Source::Riot
{
    Upstream::Upstream(IUpstream::FIPtr Interface) :
        IUpstream(Interface)
    {
    }

    void Upstream::Authenticate()
    {
    }

    Ptr<std::vector<UpstreamApp>> Upstream::GetAvailableApps(bool ForceRefresh)
    {
        if (AvailableApps && !ForceRefresh)
        {
            return AvailableApps;
        }
        auto Resp = Web::Http::Get<Web::Json::Document>(cpr::Url { "https://clientconfig.rpg.riotgames.com/api/v1/config/public" });
        if (!Resp)
        {
            throw Error::FailedHttpRequest;
        }
        ClientConfig = std::move(Resp.Get());
        AvailableApps = std::make_shared<std::vector<UpstreamApp>>();

        auto GetApp = [this](std::u8string_view Name) -> UpstreamApp& {
            auto Itr = std::find_if(AvailableApps->begin(), AvailableApps->end(), [Name](const UpstreamApp& App) {
                return std::reinterpret_pointer_cast<UpstreamAppCtx>(App.Context)->InternalName == Name;
            });
            if (Itr != AvailableApps->end())
            {
                return *Itr;
            }
            auto& Ret = AvailableApps->emplace_back();
            Ret.Name = Name;
            Ret.Context = std::make_shared<UpstreamAppCtx>(UpstreamAppCtx { std::u8string(Name) });
            return Ret;
        };

        for (const auto& Pair : ClientConfig.GetObject())
        {
            std::u8string_view KeyView = { Pair.name.GetString(), Pair.name.GetStringLength() };
            if (!KeyView.starts_with(u8"keystone.products."))
            {
                continue;
            }
            KeyView.remove_prefix(18);
            auto DotIdx = KeyView.find(u8'.');
            if (DotIdx == std::u8string_view::npos)
            {
                continue;
            }
            auto Name = KeyView.substr(0, DotIdx);
            KeyView.remove_prefix(DotIdx + 1);
            auto& App = GetApp(Name);
            if (KeyView == u8"full_name")
            {
                if (Pair.value.IsString())
                {
                    App.Name = { Pair.value.GetString(), Pair.value.GetStringLength() };
                }
                continue;
            }
            if (!KeyView.starts_with(u8"patchlines."))
            {
                continue;
            }
            KeyView.remove_prefix(11);
            auto PatchlineName = KeyView;
            Patchline PatchlineData;
            if (PatchlineData.Parse(Pair.value))
            {
                auto& Environment = App.Environments.emplace_back();
                Environment.Environment = PatchlineName;
                Environment.VersionNumeric = -1;
                Environment.Context = std::make_shared<Patchline>(std::move(PatchlineData));
            }
        }
        return AvailableApps;
    }

    const UpstreamApp* Upstream::CheckForArchiveUpdate(Ptr<IArchive> ArchivePtr)
    {
        if (ArchivePtr.expired())
        {
            return nullptr;
        }
        auto Archive = ArchivePtr.lock();

        for (auto& App : *GetAvailableApps(false).lock())
        {
            //if (Archive->
        }
    }

    OwningPtr<IUpdater> Upstream::OpenUpdater(IUpdater::FIPtr Interface) const
    {
        return OwningPtr<IUpdater>();
    }
}