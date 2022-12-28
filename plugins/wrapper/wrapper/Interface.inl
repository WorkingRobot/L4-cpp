#ifndef PLUGIN_IMPL
#error Make sure PLUGIN_IMPL is set to a valid class name.
#endif

#include <libL4/Interface.h>

static_assert(std::derived_from<PLUGIN_IMPL, ::L4::Plugin::Wrapper::IPlugin>, "PLUGIN_IMPL must be derived from IPlugin");

L4_EXPORT libL4::Initializer GetInitializer()
{
    return +[](libL4::Handle ClientHandle, const libL4::ClientInterface* Interface, libL4::PluginInterface* OutInterface) -> void {
        static std::optional<PLUGIN_IMPL> Singleton;

        if (Singleton.has_value())
        {
            throw std::invalid_argument("Already initialized");
        }
        Singleton.emplace(ClientHandle, Interface);
        *OutInterface = Singleton->GetRawInterface();
    };
}
