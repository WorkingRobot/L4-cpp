#include "InterfaceManager.h"

#include <web/Http.h>

#include <filesystem>
#include <stdexcept>

namespace L4::Riot
{
    InterfaceManager::InterfaceManager(const Source::L4Interface& Interface) :
        Interface(Interface)
    {
    }

    void InterfaceManager::SetConfigDirectory(const std::filesystem::path& ConfigDirectory)
    {
        if (!ConfigDirectory.is_absolute())
        {
            throw std::invalid_argument("ConfigDirectory is not an absolute path");
        }
        if (!std::filesystem::is_directory(ConfigDirectory))
        {
            throw std::invalid_argument("ConfigDirectory is not a directory");
        }

        this->ConfigDirectory = ConfigDirectory;
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

        return GetSourceInterface();
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

    void InterfaceManager::InterfaceWrapper::ConfigOpen(Source::String ConfigDirectory)
    {
        auto& Manager = ManagerSingleton.value();

        Manager.SetConfigDirectory(DeserializeString(ConfigDirectory));
    }

    void InterfaceManager::InterfaceWrapper::GetUserIdentity(Source::UserIdentity* OutIdentity)
    {
        auto& Manager = ManagerSingleton.value();

        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        return Manager.GetUserIdentity(*OutIdentity);
    }

    uint32_t InterfaceManager::InterfaceWrapper::GetInitialAuthMethods(const Source::AuthMethod** AuthMethods)
    {
        auto& Manager = ManagerSingleton.value();

        if (AuthMethods == nullptr)
        {
            throw std::invalid_argument("AuthMethods is null");
        }

        auto& AuthMethodVector = Manager.GetInitialAuthMethods();

        *AuthMethods = AuthMethodVector.data();
        return AuthMethodVector.size();
    }

    bool InterfaceManager::InterfaceWrapper::FulfillAuthMethod(const Source::AuthMethodFulfilled* FulfilledAuthMethod, Source::AuthMethod* NextAuthMethod)
    {
        auto& Manager = ManagerSingleton.value();

        if (FulfilledAuthMethod == nullptr)
        {
            throw std::invalid_argument("FulfilledAuthMethod is null");
        }

        if (NextAuthMethod == nullptr)
        {
            throw std::invalid_argument("NextAuthMethod is null");
        }

        return Manager.FulfillAuthMethod(*FulfilledAuthMethod, *NextAuthMethod);
    }

    uint32_t InterfaceManager::InterfaceWrapper::GetAvailableApps(const Source::AppIdentity** Apps)
    {
        auto& Manager = ManagerSingleton.value();

        if (Apps == nullptr)
        {
            throw std::invalid_argument("Apps is null");
        }

        auto& AppVector = Manager.GetAvailableApps();

        *Apps = AppVector.data();
        return AppVector.size();
    }

    bool InterfaceManager::InterfaceWrapper::IsValidApp(const Source::AppIdentity* Identity)
    {
        auto& Manager = ManagerSingleton.value();

        if (Identity == nullptr)
        {
            throw std::invalid_argument("Identity is null");
        }

        return Manager.IsValidApp(*Identity);
    }

    void InterfaceManager::InterfaceWrapper::UpdateOpen(Source::Update Update, const Source::AppIdentity* OldIdentity, Source::AppIdentity* NewIdentity)
    {
        auto& Manager = ManagerSingleton.value();

        if (Update == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        if (OldIdentity == nullptr)
        {
            throw std::invalid_argument("OldIdentity is null");
        }

        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        if (Manager.Updates.contains(Update))
        {
            throw std::invalid_argument("Update already exists");
        }

        Manager.Updates.emplace(std::piecewise_construct, std::forward_as_tuple(Update), std::forward_as_tuple(*OldIdentity, *NewIdentity));
    }

    void InterfaceManager::InterfaceWrapper::UpdateClose(Source::Update Update)
    {
        auto& Manager = ManagerSingleton.value();

        if (Update == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        if (Manager.Updates.erase(Update) == 0)
        {
            throw std::invalid_argument("Update does not exist");
        }
    }

    void InterfaceManager::InterfaceWrapper::UpdateStart(Source::Update Update, Source::Archive Archive, uint32_t ProgressUpdateRateMs)
    {
        auto& Manager = ManagerSingleton.value();

        if (Update == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        auto UpdateItr = Manager.Updates.find(Update);
        if (UpdateItr == Manager.Updates.end())
        {
            throw std::invalid_argument("Update does not exist");
        }

        UpdateItr->second.SetCallbacks({ .GetState = [Update]() -> Source::UpdateState { return ManagerSingleton.value().Interface.UpdateGetState(Update); },
                                         .OnStart = [Update](const Source::UpdateStartInfo& StartInfo) { ManagerSingleton.value().Interface.UpdateOnStart(Update, &StartInfo); },
                                         .OnProgress = [Update](const Source::UpdateProgressInfo& ProgressInfo) { ManagerSingleton.value().Interface.UpdateOnProgress(Update, &ProgressInfo); },
                                         .OnPieceUpdate = [Update](uint64_t Id, Source::UpdatePieceStatus NewStatus) { ManagerSingleton.value().Interface.UpdateOnPieceUpdate(Update, Id, NewStatus); },
                                         .OnFinalize = [Update]() { ManagerSingleton.value().Interface.UpdateOnFinalize(Update); },
                                         .OnComplete = [Update]() { ManagerSingleton.value().Interface.UpdateOnComplete(Update); } });

        UpdateItr->second.Start(Archive, std::chrono::milliseconds(ProgressUpdateRateMs));
    }

    void InterfaceManager::InterfaceWrapper::UpdatePause(Source::Update Update)
    {
        auto& Manager = ManagerSingleton.value();

        if (Update == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        auto UpdateItr = Manager.Updates.find(Update);
        if (UpdateItr == Manager.Updates.end())
        {
            throw std::invalid_argument("Update does not exist");
        }

        UpdateItr->second.Pause();
    }

    void InterfaceManager::InterfaceWrapper::UpdateResume(Source::Update Update)
    {
        auto& Manager = ManagerSingleton.value();

        if (Update == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        auto UpdateItr = Manager.Updates.find(Update);
        if (UpdateItr == Manager.Updates.end())
        {
            throw std::invalid_argument("Update does not exist");
        }

        UpdateItr->second.Resume();
    }
}