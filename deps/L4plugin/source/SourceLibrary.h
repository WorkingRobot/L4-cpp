#pragma once

#include "SourceStructs.h"
#include "SourceUpdate.h"

#include <filesystem>

namespace L4
{
    class SourceLibrary
    {
    public:
        SourceLibrary(const std::filesystem::path& FilePath);

        ~SourceLibrary();

        bool Initialize(const Source::L4Interface& Interface);

        const Source::SourceIdentity& GetIdentity() const;

        void ProvideConfig(const std::filesystem::path& ConfigDirectory);

        const std::vector<Source::AppIdentity>& GetAvailableApps();

        bool IsValidApp(const Source::AppIdentity& Identity) const;

        SourceUpdate OpenUpdate(const Source::AppIdentity& Identity) const;

    private:
        friend SourceUpdate;

        using InitializeProc = const Source::SourceInterface* (*)(const Source::L4Interface* Interface);

        void* ModuleHandle;
        InitializeProc InitProc;
        Source::SourceInterface Interface;
        std::u8string ConfigDirectory;
        std::vector<Source::AppIdentity> AvailableApps;
    };
}