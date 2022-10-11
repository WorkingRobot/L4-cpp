#pragma once

#include "Factory.h"
#include "Update.h"

#include <filesystem>
#include <optional>
#include <unordered_map>

namespace L4::SourceWrapper
{
    class InterfaceManager
    {
    public:
        InterfaceManager(const Source::L4Interface& Interface);

        void GetUserIdentity(Source::UserIdentity& OutIdentity);
        const std::vector<Source::AuthMethod>& GetInitialAuthMethods();
        bool FulfillAuthMethod(const Source::AuthMethodFulfilled& FulfilledAuthMethod, Source::AuthMethod& NextAuthMethod);

        const std::vector<Source::AppIdentity>& GetAvailableApps();
        bool IsValidApp(const Source::AppIdentity& Identity);

        static const Source::SourceInterface& Initialize(const Source::L4Interface& Interface);

    private:
        struct InterfaceWrapper
        {
            static void GetUserIdentity(Source::UserIdentity* OutIdentity);
            static uint32_t GetInitialAuthMethods(const Source::AuthMethod** AuthMethods);
            static bool FulfillAuthMethod(const Source::AuthMethodFulfilled* FulfilledAuthMethod, Source::AuthMethod* NextAuthMethod);

            static uint32_t GetAvailableApps(const Source::AppIdentity** Apps);
            static bool IsValidApp(const Source::AppIdentity* Identity);

            static void UpdateOpen(Source::Update Update, const Source::AppIdentity* OldIdentity, Source::AppIdentity* NewIdentity);
            static void UpdateClose(Source::Update Update);
            static void UpdateStart(Source::Update Update, Source::Archive Archive, uint32_t ProgressUpdateRateMs);
            static void UpdatePause(Source::Update Update);
            static void UpdateResume(Source::Update Update);
        };

        Source::SourceInterface CreateSourceInterface() const noexcept;

        static std::optional<InterfaceManager> ManagerSingleton;

        Source::L4Interface Interface;
        std::unique_ptr<IInterface> Base;

        VersionedIdentity SourceIdentity;
        Source::SourceInterface SourceInterface;
        std::filesystem::path ConfigDirectory;
        std::vector<Source::AppIdentity> AvailableApps;
        std::unordered_map<Source::Update, Update> Updates;
    };
}