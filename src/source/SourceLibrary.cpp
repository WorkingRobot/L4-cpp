#include "SourceLibrary.h"

#include "utils/Error.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    SourceLibrary::SourceLibrary(const std::filesystem::path& FilePath)
    {
        ModuleHandle = LoadLibraryW(FilePath.c_str());
        if (ModuleHandle == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        InitProc = (InitializeProc)GetProcAddress((HMODULE)ModuleHandle, "Initialize");
        if (InitProc == NULL)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
    }

    SourceLibrary::~SourceLibrary()
    {
        FreeLibrary((HMODULE)ModuleHandle);
    }

    bool SourceLibrary::Initialize(const Source::L4Interface& Interface)
    {
        auto InterfacePtr = InitProc(&Interface);
        if (!InterfacePtr)
        {
            return false;
        }
        this->Interface = *InterfacePtr;
        return true;
    }

    const Source::SourceIdentity& SourceLibrary::GetIdentity() const
    {
        return Interface.Identity;
    }

    void SourceLibrary::ProvideConfig(const std::filesystem::path& ConfigDirectory)
    {
        if (!ConfigDirectory.is_absolute())
        {
            throw std::invalid_argument("ConfigDirectory is not an absolute path"); 
        }
        if (!std::filesystem::is_directory(ConfigDirectory))
        {
            throw std::invalid_argument("ConfigDirectory is not a directory");
        }
        this->ConfigDirectory = ConfigDirectory.u8string();
        Interface.ConfigOpen(SerializeString(this->ConfigDirectory));
    }

    const std::vector<Source::AppIdentity>& SourceLibrary::GetAvailableApps()
    {
        AvailableApps.clear();

        const Source::AppIdentity* Apps;
        uint32_t AppCount = Interface.GetAvailableApps(&Apps);
        if (Apps == nullptr)
        {
            return AvailableApps;
        }
        if (AppCount == 0)
        {
            AvailableApps.clear();
        }
        else
        {
            AvailableApps.resize(AppCount);
            std::copy_n(Apps, AppCount, AvailableApps.begin());
        }

        return AvailableApps;
    }

    bool SourceLibrary::IsValidApp(const Source::AppIdentity& Identity) const
    {
        return Interface.IsValidApp(&Identity);
    }

    SourceUpdate SourceLibrary::OpenUpdate(const Source::AppIdentity& Identity) const
    {
        return SourceUpdate(*this, Identity);
    }
}