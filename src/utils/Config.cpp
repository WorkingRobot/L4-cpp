#include "Config.h"

#include <cstdlib>

namespace L4::Config
{
    const std::filesystem::path& GetFolder() noexcept
    {
        static const auto Path = GetExeFolder();
        return Path;
    }

    static inline std::filesystem::path GetExePathInternal() noexcept
    {
        char* Path;
        if (_get_pgmptr(&Path) == 0)
        {
            std::error_code Error;
            return std::filesystem::absolute(Path, Error);
        }
        return "";
    }

    const std::filesystem::path& GetExePath() noexcept
    {
        static const auto Path = GetExePathInternal();
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

    const char* GetVersionHash() noexcept
    {
        return CONFIG_VERSION_HASH;
    }

    const char* GetVersionBranch() noexcept
    {
        return CONFIG_VERSION_BRANCH;
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