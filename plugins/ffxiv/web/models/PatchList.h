#pragma once

#include <utils/ParseNum.h>

#include <array>
#include <ranges>

namespace L4::Plugin::FFXIV::Models
{
    namespace Detail
    {
        static constexpr auto ToStringView = std::views::transform([](auto Entry) {
            return std::string_view(Entry.begin(), Entry.end());
        });

        template <int PropCount>
        static constexpr auto ToSplitArray = std::views::transform([](std::string_view Entry) {
            std::array<std::string_view, PropCount> Ret;
            std::ranges::copy(Entry | std::views::split('\t') | std::views::take(PropCount) | ToStringView, Ret.begin());
            return Ret;
        });

        template <int PropCount>
        static constexpr auto RespToEntries =
            std::views::split(std::string_view("\r\n")) |
            std::views::drop(5) |
            ToStringView |
            std::views::take_while([](std::string_view Entry) { return !Entry.starts_with("--"); }) |
            ToSplitArray<PropCount>;
    }

    struct PatchListBoot
    {
        struct Entry
        {
            int64_t DownloadSize;
            int64_t InstalledSize;
            int64_t FileCount;
            int64_t FileCount2;
            std::string Version;
            std::string Url;
        };

        std::vector<Entry> Entries;

        PatchListBoot(std::string_view Data)
        {
            static constexpr auto ToEntry = std::views::transform([&](std::array<std::string_view, 6> Views) {
                return Entry {
                    .DownloadSize = ParseNum<int64_t>(Views[0]).value_or(0),
                    .InstalledSize = ParseNum<int64_t>(Views[1]).value_or(0),
                    .FileCount = ParseNum<int64_t>(Views[2]).value_or(0),
                    .FileCount2 = ParseNum<int64_t>(Views[3]).value_or(0),
                    .Version = std::string(Views[4]),
                    .Url = std::string(Views[5]),
                };
            });
            std::ranges::copy(Data | Detail::RespToEntries<6> | ToEntry, std::back_inserter(Entries));
        }
    };

    struct PatchListGame
    {
        struct Entry
        {
            int64_t DownloadSize;
            int64_t InstalledSize;
            int64_t FileCount;
            int64_t FileCount2;
            std::string VersionId;
            std::string HashType;
            int64_t HashBlockSize;
            std::string Hashes;
            std::string Url;
        };

        std::vector<Entry> Entries;

        PatchListGame(std::string_view Data)
        {
            static constexpr auto ToEntry = std::views::transform([&](std::array<std::string_view, 9> Views) {
                return Entry {
                    .DownloadSize = ParseNum<int64_t>(Views[0]).value_or(0),
                    .InstalledSize = ParseNum<int64_t>(Views[1]).value_or(0),
                    .FileCount = ParseNum<int64_t>(Views[2]).value_or(0),
                    .FileCount2 = ParseNum<int64_t>(Views[3]).value_or(0),
                    .VersionId = std::string(Views[4]),
                    .HashType = std::string(Views[5]),
                    .HashBlockSize = ParseNum<int64_t>(Views[6]).value_or(0),
                    .Hashes = std::string(Views[7]),
                    .Url = std::string(Views[8]),
                };
            });
            std::ranges::copy(Data | Detail::RespToEntries<9> | ToEntry, std::back_inserter(Entries));
        }
    };
}