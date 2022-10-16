#pragma once

#include "ClientInterface.h"
#include "IAuth.h"
#include "IUpdate.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <stdexcept>

namespace L4::Plugin::Wrapper
{
    class IPlugin
    {
    public:
        IPlugin(const libL4::ClientInterface* ClientInterface, std::unique_ptr<IAuth> AuthInterface, std::unique_ptr<IUpdate> UpdateInterface);

        virtual ~IPlugin() = default;

        libL4::PluginInterface GetRawInterface();

        virtual PluginIdentity GetIdentity() const = 0;

    private:
        friend class IAuth;
        friend class IUpdate;

        struct RawOps
        {
            struct Auth
            {
                static bool GetUser(libL4::UserIdentity* OutIdentity);

                static void Open(libL4::Handle Auth);

                static void Close(libL4::Handle Auth);

                static void GetFields(libL4::Handle Auth, libL4::AuthField Fields[16], uint32_t* FieldCount);

                static void Submit(libL4::Handle Auth, const libL4::AuthFulfilledField Fields[16], uint32_t FieldCount, libL4::AuthSubmitResponse* Response);
            };

            struct Update
            {
                static void Open(libL4::Handle Update, const libL4::AppIdentity* OldIdentity, libL4::AppIdentity* NewIdentity);

                static void Close(libL4::Handle Update);

                static void Start(libL4::Handle Update, libL4::Handle Archive, uint32_t ProgressUpdateRateMs);

                static void Pause(libL4::Handle Update);

                static void Resume(libL4::Handle Update);
            };
        };

    protected:
        ClientInterface Client;
        std::unique_ptr<IAuth> Auth;
        std::unique_ptr<IUpdate> Update;

        static IPlugin* Instance;
    };
}