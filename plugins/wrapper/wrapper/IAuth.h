#include "IAuthSession.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class IAuth
    {
    public:
        IAuth();

        virtual ~IAuth() = default;

        void OnUserUpdated();

        virtual std::optional<UserIdentity> GetUser() = 0;

        virtual std::unique_ptr<IAuthSession> CreateSession(Handle ClientHandle) = 0;
    };
}