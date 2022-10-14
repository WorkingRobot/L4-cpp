#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class IAuthSession
    {
    public:
        IAuthSession(Handle ClientHandle);

        virtual ~IAuthSession() = default;

        virtual std::vector<AuthField> GetFields() = 0;
        virtual void Submit(const std::vector<AuthFulfilledField>& Fields) = 0;
    };
}