#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <filesystem>
#include <stdexcept>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class ClientInterface
    {
    public:
        ClientInterface(const libL4::ClientInterface* Interface);

        libL4::InterfaceVersion GetInterfaceVersion() const;

        const ClientIdentity& GetIdentity() const;

        const std::filesystem::path& GetConfigDirectory() const;

    private:
        friend class Archive;
        friend class Stream;
        friend class IAuth;
        friend class IUpdateSession;

        libL4::ClientInterface Interface;
    };
}