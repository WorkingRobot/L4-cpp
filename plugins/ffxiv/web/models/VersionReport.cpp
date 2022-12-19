#include "VersionReport.h"

#include <format/Format.h>

#include <ranges>

namespace L4::Plugin::FFXIV::Models
{
    std::string VersionReport::Serialize() const
    {
        auto FormatBootFile = [](const BootFile& File) { return FMT::format("{:s}/{:d}/{:02x}", File.Name, File.Size, FMT::join(File.Hash, "")); };
        auto FormattedBootFiles = BootFiles | std::views::transform(FormatBootFile) | std::views::join_with(',') | std::ranges::to<std::string>();

        auto FormatExpacVersion = [Idx = 0](const std::string& Version) mutable { return FMT::format("ex{:d}\t{:s}", ++Idx, Version); };
        auto FormattedExpacVersions = ExpacVersions | std::views::transform(FormatExpacVersion) | std::views::join_with('\n') | std::ranges::to<std::string>();

        return std::format("{:s}={:s}\n{:s}", BootVersion, FormattedBootFiles, FormattedExpacVersions);
    }
}