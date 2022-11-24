#pragma once

#include <libL4/Interface.h>
#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Plugin::Manager
{
    class AuthSession
    {
    public:
        AuthSession(libL4::PluginInterface Interface);

        ~AuthSession();

        const std::vector<libL4::Marshal::AuthField>& GetFields();

        libL4::Marshal::AuthSubmitResponse Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields);

    private:
        std::vector<libL4::Marshal::AuthField> GetFieldsInternal();

        bool ShouldRefreshFields;
        std::vector<libL4::Marshal::AuthField> Fields;

        libL4::PluginInterface Interface;
    };
}