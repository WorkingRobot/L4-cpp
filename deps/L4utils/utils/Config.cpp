#include "Config.h"

#include <cstdlib>

namespace L4::Config
{
    const std::filesystem::path& GetFolder() noexcept
    {
        static const auto Path = GetExeFolder();
        return Path;
    }

    const std::filesystem::path& GetExeFolder() noexcept
    {
        static const auto Path = GetExePath().parent_path();
        return Path;
    }

    const char* GetProjectName() noexcept
    {
        return CONFIG_PROJECT_NAME;
    }

    uint8_t GetVersionMajor() noexcept
    {
        return CONFIG_VERSION_MAJOR;
    }

    uint8_t GetVersionMinor() noexcept
    {
        return CONFIG_VERSION_MINOR;
    }

    uint8_t GetVersionPatch() noexcept
    {
        return CONFIG_VERSION_PATCH;
    }

    uint32_t GetVersionNumeric() noexcept
    {
        return uint32_t(GetVersionMajor()) << 24 | uint32_t(GetVersionMinor()) << 16 | uint32_t(GetVersionPatch());
    }

    const char* GetVersionBranch() noexcept
    {
        return CONFIG_VERSION_BRANCH;
    }

    const char* GetVersionRevision() noexcept
    {
        return CONFIG_VERSION_REVISION;
    }

    bool GetVersionIsModified() noexcept
    {
#ifdef CONFIG_VERSION_IS_MODIFIED
        return true;
#else
        return false;
#endif
    }

    const char* GetVersionPlatform() noexcept
    {
#define STR_(v) #v
#define STR(v) STR_(v)
        return STR(CONFIG_VERSION_PLATFORM);
#undef STR
#undef STR_
    }

    const char* GetVersion() noexcept
    {
        return CONFIG_VERSION;
    }

    const char* GetVersionLong() noexcept
    {
        return CONFIG_VERSION_LONG;
    }
}