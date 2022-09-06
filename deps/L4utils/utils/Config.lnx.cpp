#include "Config.h"

namespace L4::Config
{
    static inline std::filesystem::path GetExePathInternal() noexcept
    {
        std::error_code Error;
        auto Path = std::filesystem::read_symlink("/proc/self/exe", Error);
        if (!Error) {
            return std::filesystem::absolute(Path, Error);
        }
        return "";
    }

    const std::filesystem::path& GetExePath() noexcept
    {
        static const auto Path = GetExePathInternal();
        return Path;
    }
}