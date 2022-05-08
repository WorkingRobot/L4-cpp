#include "Config.h"

#include <cstdlib>

namespace L4::Config
{
    const std::filesystem::path& GetFolder() noexcept
    {
        static const auto Path = GetExeFolder();
        return Path;
    }

    inline std::filesystem::path GetExePathInternal() noexcept
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
}