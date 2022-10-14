#ifndef PLUGIN_IMPL
#error Make sure PLUGIN_IMPL is set to a valid class name.
#endif

#include <stdexcept>

static_assert(std::derived_from<PLUGIN_IMPL, ::L4::Plugin::Wrapper::IPlugin>, "PLUGIN_IMPL must be derived from IPlugin");

std::optional<PLUGIN_IMPL> Singleton;

bool Initialize(const libL4::ClientInterface* Interface, libL4::PluginInterface* OutInterface)
{
    try
    {
        if (Singleton.has_value())
        {
            throw std::invalid_argument("Already initialized");
        }
        Singleton.emplace(Interface);
        *OutInterface = Singleton->GetRawInterface();
        return true;
    }
    catch (...)
    {
        // extern "C" functions throwing C++ exceptions is undefined behavior
        return false;
    }
}