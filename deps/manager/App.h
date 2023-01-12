#pragma once

#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Manager
{
    class App
    {
    public:
        App(const std::filesystem::path& Path);

        App(const libL4::Marshal::ArchiveIdentity& Identity, const std::filesystem::path& Path, uint32_t SectorSize, uint32_t MaxStreamCount);

        const libL4::Marshal::ArchiveIdentity& GetIdentity() const noexcept;

    private:
        std::filesystem::path Path;
        libL4::Marshal::ArchiveIdentity Identity;
    };
}