#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <filesystem>

namespace L4::Plugin::Wrapper
{
    class ClientInterface
    {
    public:
        ClientInterface(const libL4::ClientInterface* Interface);

        libL4::InterfaceVersion GetInterfaceVersion() const noexcept;

        const libL4::Marshal::ClientIdentity& GetIdentity() const noexcept;

        const std::filesystem::path& GetConfigDirectory() const noexcept;

        libL4::ClientInterface* operator->() noexcept;

    private:
        libL4::ClientInterface Interface;

        libL4::Marshal::ClientIdentity Identity;
        std::filesystem::path ConfigDirectory;
    };
}