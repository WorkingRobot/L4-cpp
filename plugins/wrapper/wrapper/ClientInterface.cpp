#include "ClientInterface.h"

namespace L4::Plugin::Wrapper
{
    ClientInterface::ClientInterface(const libL4::ClientInterface* Interface)
    {
        if (Interface == nullptr)
        {
            throw std::invalid_argument("Interface is null");
        }
    }
}