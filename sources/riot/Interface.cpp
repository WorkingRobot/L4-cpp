#include "InterfaceManager.h"

#include <L4/source/Interface.h>

#include <stdexcept>

const L4::Source::SourceInterface* Initialize(const L4::Source::L4Interface* Interface)
{
    try
    {
        if (Interface == nullptr)
        {
            throw std::invalid_argument("Interface is null");
        }

        return &L4::Riot::InterfaceManager::Initialize(*Interface);
    }
    catch (...)
    {
        // extern "C" functions throwing C++ exceptions is undefined behavior
        return nullptr;
    }
}
