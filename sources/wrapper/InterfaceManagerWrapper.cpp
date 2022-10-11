#include "InterfaceManager.h"

#include <stdexcept>

namespace L4::SourceWrapper
{
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
