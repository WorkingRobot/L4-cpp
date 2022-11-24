#pragma once

#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Plugin::Manager
{
    class IManager
    {
    public:
        IManager() = default;

        virtual libL4::Marshal::ClientIdentity GetIdentity() = 0;

        void Load(const std::filesystem::path& FilePath);

    private:
    };
}