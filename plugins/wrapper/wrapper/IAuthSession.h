#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    class IAuthSession
    {
    public:
        IAuthSession() = default;

        virtual ~IAuthSession() = default;

        virtual std::vector<libL4::Marshal::AuthField> GetFields() = 0;
        virtual libL4::Marshal::AuthSubmitResponse Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields) = 0;
    };
}