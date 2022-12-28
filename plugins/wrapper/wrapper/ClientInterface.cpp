#include "ClientInterface.h"

namespace L4::Plugin::Wrapper
{
    ClientInterface::ClientInterface(const libL4::ClientInterface* Interface)
    {
        if (Interface == nullptr)
        {
            throw std::invalid_argument("Interface is null");
        }

        this->Interface = *Interface;
        Identity = libL4::Marshal::To(Interface->Identity);
        ConfigDirectory = libL4::Marshal::To(Interface->ConfigBaseDirectory);
    }

    libL4::InterfaceVersion ClientInterface::GetInterfaceVersion() const noexcept
    {
        return Interface.InterfaceVersion;
    }

    const libL4::Marshal::ClientIdentity& ClientInterface::GetIdentity() const noexcept
    {
        return Identity;
    }

    const std::filesystem::path& ClientInterface::GetConfigDirectory() const noexcept
    {
        return ConfigDirectory;
    }

    libL4::ClientInterface* ClientInterface::operator->() noexcept
    {
        return &Interface;
    }
}