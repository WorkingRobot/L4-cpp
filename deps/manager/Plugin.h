#pragma once

#include "AuthSession.h"
#include "ClientInterface.h"
#include "UpdateSession.h"
#include "utils/Callback.h"
#include "utils/EnableSelfUniqueFromThis.h"
#include "utils/RAIIWrapper.h"

#include <libL4/Interface.h>
#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Manager
{
    class Plugin : public EnableSelfUniqueFromThis<Plugin>
    {
    public:
        Plugin(const std::filesystem::path& Path);

        void Initialize(const ClientInterface& ClientInterface);

        const libL4::Marshal::PluginIdentity& GetIdentity() const noexcept;

        const libL4::PluginInterface& GetRawInterface() const;

        std::optional<libL4::Marshal::UserIdentity> AuthGetUser();

        std::unique_ptr<AuthSession> AuthOpenSession();

        std::unique_ptr<UpdateSession> UpdateOpenSession(const libL4::Marshal::AppIdentity& Identity);

        Callback<void()> OnAuthUserUpdated;

    private:
        static void* LoadPlugin(const std::filesystem::path& Path);
        static decltype(&::GetInitializer) GetInitializerFunc(void* OSHandle);
        static void UnloadPlugin(void* OSHandle);

        RAIIWrapper<void*, UnloadPlugin> OSHandle;

        libL4::PluginInterface RawInterface;

        libL4::Marshal::PluginIdentity Identity;
    };
}