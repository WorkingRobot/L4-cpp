#pragma once

#include <array>
#include <string>
#include <unordered_map>

namespace L4::Plugin::FFXIV::Models
{
    struct VersionReport
    {
        struct BootFile
        {
            std::string Name;
            uint64_t Size;
            std::array<std::byte, 20> Hash;
        };

        std::string BootVersion;
        std::vector<BootFile> BootFiles;

        std::vector<std::string> ExpacVersions;

        std::string Serialize() const;
    };
}