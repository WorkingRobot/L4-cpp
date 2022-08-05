#pragma once

#include "Archive.h"
#include "Base.h"
#include "Guid.h"
#include "Updater.h"

#include <vector>

namespace L4::Interface
{
    struct UpstreamAppEnvironment
    {
        std::u8string Environment;
        std::u8string Version;
        uint64_t VersionNumeric;
        OwningPtr<void> Context;
    };

    struct UpstreamApp
    {
        Guid Guid;
        std::u8string Name;
        std::vector<UpstreamAppEnvironment> Environments;
        OwningPtr<void> Context;
    };

    class L4_CLASS_API IUpstream : public Object<>
    {
    protected:
        using Object::Object;

    public:
        virtual void Authenticate() = 0;

        virtual Ptr<std::vector<UpstreamApp>> GetAvailableApps(bool ForceRefresh) = 0;

        virtual const UpstreamApp* CheckForArchiveUpdate(Ptr<IArchive> Archive) = 0;

        virtual OwningPtr<IUpdater> OpenUpdater(IUpdater::FIPtr Interface) const = 0;
    };
}