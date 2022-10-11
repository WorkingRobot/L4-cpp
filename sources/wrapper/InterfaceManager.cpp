#include "InterfaceManager.h"

#include <filesystem>
#include <stdexcept>

namespace L4::SourceWrapper
{
    L4Identity DeserializeL4Identity(const Source::L4Identity& Identity)
    {
        return L4Identity {
            .Id = std::u8string(DeserializeString(Identity.Id)),
            .Name = std::u8string(DeserializeString(Identity.Name)),
            .Version = {
                .Humanized = std::u8string(DeserializeString(Identity.Version.Humanized)),
                .Numeric = Identity.Version.Numeric,
            }
        };
    }

    InterfaceManager::InterfaceManager(const Source::L4Interface& Interface) :
        Interface(Interface),
        Base(Factory::CreateInterface(DeserializeL4Identity(Interface.Identity))),
        SourceIdentity(Base->GetIdentity()),
        SourceInterface {
            .Identity = {
                .Id = SerializeString(SourceIdentity.Id),
                .Name = SerializeString(SourceIdentity.Name),
                .Version = {
                    .Humanized = SerializeString(SourceIdentity.Version.Humanized),
                    .Numeric = SourceIdentity.Version.Numeric } },
            .GetUserIdentity = &InterfaceWrapper::GetUserIdentity,
            .GetInitialAuthMethods = &InterfaceWrapper::GetInitialAuthMethods,
            .FulfillAuthMethod = &InterfaceWrapper::FulfillAuthMethod,
            .GetAvailableApps = &InterfaceWrapper::GetAvailableApps,
            .IsValidApp = &InterfaceWrapper::IsValidApp,
            .UpdateOpen = &InterfaceWrapper::UpdateOpen,
            .UpdateClose = &InterfaceWrapper::UpdateClose,
            .UpdateStart = &InterfaceWrapper::UpdateStart,
            .UpdatePause = &InterfaceWrapper::UpdatePause,
            .UpdateResume = &InterfaceWrapper::UpdateResume
        }
    {
        ConfigDirectory = DeserializeString(Interface.ConfigBaseDirectory);
        if (!ConfigDirectory.is_absolute())
        {
            throw std::invalid_argument("ConfigBaseDirectory is not an absolute path");
        }
        if (!std::filesystem::is_directory(ConfigDirectory))
        {
            throw std::invalid_argument("ConfigBaseDirectory is not a directory");
        }
        ConfigDirectory /= DeserializeString(GetSourceInterface().Identity.Id);
        std::filesystem::create_directory(ConfigDirectory);

        auto Identity = Base->GetIdentity();
        SourceInterface.Identity.Id =
    }

    void InterfaceManager::GetUserIdentity(Source::UserIdentity& OutIdentity)
    {
    }

    const std::vector<Source::AuthMethod>& InterfaceManager::GetInitialAuthMethods()
    {
        // TODO: insert return statement here
    }

    bool InterfaceManager::FulfillAuthMethod(const Source::AuthMethodFulfilled& FulfilledAuthMethod, Source::AuthMethod& NextAuthMethod)
    {
        return false;
    }

    const std::vector<Source::AppIdentity>& InterfaceManager::GetAvailableApps()
    {
        return AvailableApps;
    }

    bool InterfaceManager::IsValidApp(const Source::AppIdentity& Identity)
    {
        return false;
    }

    std::optional<InterfaceManager> InterfaceManager::ManagerSingleton;

    const Source::SourceInterface& InterfaceManager::Initialize(const Source::L4Interface& Interface)
    {
        if (ManagerSingleton.has_value())
        {
            throw std::invalid_argument("Source is already initialized");
        }

        printf("Initializing source with %s %s (%x)\n", (const char*)Interface.Identity.Name.Data, (const char*)Interface.Identity.Version.Humanized.Data, Interface.Identity.Version.Numeric);
        ManagerSingleton.emplace(Interface);

        ManagerSingleton->Base->return GetSourceInterface();
    }

    Source::SourceInterface InterfaceManager::CreateSourceInterface() noexcept
    {
        return Source::SourceInterface;
    }

    const Source::SourceInterface& InterfaceManager::GetSourceInterface() noexcept
    {
        static const Source::SourceInterface Interface {
            .Identity = {
                .Id = SerializeString(u8"riot"),
                .Name = SerializeString(u8"Riot Games"),
                .Version = {
                    .Humanized = SerializeString(u8"0.0.1"),
                    .Numeric = 1 } },

            .ConfigOpen = &InterfaceWrapper::ConfigOpen,
        };

        return Interface;
    }
}