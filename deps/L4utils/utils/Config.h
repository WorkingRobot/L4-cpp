#pragma once

#include <filesystem>

namespace L4::Config
{
    const std::filesystem::path& GetFolder() noexcept;

    const std::filesystem::path& GetExePath() noexcept;

    const std::filesystem::path& GetExeFolder() noexcept;

    const char* GetProjectName() noexcept;

    uint8_t GetVersionMajor() noexcept;
    uint8_t GetVersionMinor() noexcept;
    uint8_t GetVersionPatch() noexcept;
    uint32_t GetVersionNumeric() noexcept;

    const char* GetVersionBranch() noexcept;
    const char* GetVersionRevision() noexcept;
    bool GetVersionIsModified() noexcept;
    const char* GetVersionPlatform() noexcept;
    const char* GetVersion() noexcept;
    const char* GetVersionLong() noexcept;
}