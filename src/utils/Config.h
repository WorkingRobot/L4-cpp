#pragma once

#include <filesystem>

namespace L4::Config
{
    const std::filesystem::path& GetFolder() noexcept;

    const std::filesystem::path& GetExePath() noexcept;

    const std::filesystem::path& GetExeFolder() noexcept;
}