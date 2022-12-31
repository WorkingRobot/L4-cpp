#include "Plugin.h"

namespace L4::Manager
{
    Plugin::Plugin(const std::filesystem::path& Path) :
        OSHandle(LoadPlugin(Path))
    {
        
    }

    void Plugin::Initialize(const ClientInterface& ClientInterface)
    {
        auto GetInitializer = GetInitializerFunc(OSHandle)();
        if (GetInitializer == nullptr)
        {
            throw std::runtime_error("GetInitializer returned null");
        }

        GetInitializer(libL4::Handle(GetSelfPtr()), &ClientInterface.GetRawInterface(), &RawInterface);

        Identity = libL4::Marshal::To(RawInterface.Identity);
    }

    const libL4::Marshal::PluginIdentity& Plugin::GetIdentity() const noexcept
    {
        return Identity;
    }

    const libL4::PluginInterface& Plugin::GetRawInterface() const
    {
        return RawInterface;
    }
}