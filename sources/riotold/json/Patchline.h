#pragma once

#include <L4/web/JsonMacros.h>

namespace L4::Source::Riot
{
    struct Patchline
    {
        struct Locales
        {
            std::vector<JsonString> AvailableLocales;
            JsonString DefaultLocale;

            JSON_DEFINE
                JSON_ITEM("available_locales", AvailableLocales)
                JSON_ITEM("default_locale", DefaultLocale)
            JSON_DEFINE_END
        };

        struct Regions
        {
            std::vector<JsonString> AvailableRegions;
            JsonString DefaultRegion;

            JSON_DEFINE
                JSON_ITEM("available_regions", AvailableRegions)
                JSON_ITEM("default_region", DefaultRegion)
            JSON_DEFINE_END
        };

        struct Alias
        {
            std::vector<JsonString> Platforms;
            JsonString ProductId;

            JSON_DEFINE
                JSON_ITEM_NULL("platforms", Platforms)
                JSON_ITEM("product_id", ProductId)
            JSON_DEFINE_END
        };

        struct ContentPaths
        {
            JsonString Loc;
            JsonString RiotStatus;
            JsonString Social;
            JsonString Themes;

            JSON_DEFINE
                JSON_ITEM("loc", Loc)
                JSON_ITEM_OPT("riotstatus", RiotStatus)
                JSON_ITEM_OPT("social", Social)
                JSON_ITEM_OPT("themes", Themes)
            JSON_DEFINE_END
        };

        struct Meta
        {
            Alias Alias;
            std::vector<JsonString> AvailablePlatforms;
            JsonString ClientProductType;
            ContentPaths ContentPaths;
            JsonString DefaultThemeManifest;
            JsonString FullName;
            JsonString PathName;
            JsonString SupportedVersions;
            JsonString ThemeManifest;

            JSON_DEFINE
                JSON_ITEM("alias", Alias)
                JSON_ITEM_OPT("available_platforms", AvailablePlatforms)
                JSON_ITEM_OPT("client_product_type", ClientProductType)
                JSON_ITEM_OPT("content_paths", ContentPaths)
                JSON_ITEM_OPT("default_theme_manifest", DefaultThemeManifest)
                JSON_ITEM_OPT("full_name", FullName)
                JSON_ITEM_OPT("path_name", PathName)
                JSON_ITEM_OPT("supported_versions", SupportedVersions)
                JSON_ITEM_OPT("theme_manifest", ThemeManifest)
            JSON_DEFINE_END
        };

        struct Executables
        {
            JsonString App;
            JsonString Exe;

            JSON_DEFINE
                JSON_ITEM("app", App)
                JSON_ITEM("exe", Exe)
            JSON_DEFINE_END
        };

        struct Launcher
        {
            std::vector<JsonString> Arguments;
            JsonString ComponentId;
            Executables Executables;

            JSON_DEFINE
                JSON_ITEM("arguments", Arguments)
                JSON_ITEM("component_id", ComponentId)
                JSON_ITEM("executables", Executables)
            JSON_DEFINE_END
        };

        struct Shards
        {
            std::vector<JsonString> Live;

            JSON_DEFINE
                JSON_ITEM("live", Live)
            JSON_DEFINE_END
        };

        struct Dependency
        {
            std::vector<JsonString> Args;
            bool Elevate;
            JsonString Hash;
            JsonString Id;
            JsonString MinVersion;
            JsonString Url;
            JsonString Version;

            JSON_DEFINE
                JSON_ITEM_NULL("args", Args)
                JSON_ITEM("elevate", Elevate)
                JSON_ITEM("hash", Hash)
                JSON_ITEM("id", Id)
                JSON_ITEM("min_version", MinVersion)
                JSON_ITEM("url", Url)
                JSON_ITEM("version", Version)
            JSON_DEFINE_END
        };

        struct SecondaryPatchline
        {
            std::vector<JsonString> AllowedHttpFallbackHostnames;
            JsonString BundlesUrl;
            std::vector<JsonString> ExcludedPaths;
            JsonString Id;
            JsonString Path;
            std::vector<JsonString> Tags;
            JsonString Url;

            JSON_DEFINE
                JSON_ITEM_NULL("allowed_http_fallback_hostnames", AllowedHttpFallbackHostnames)
                JSON_ITEM("bundles_url", BundlesUrl)
                JSON_ITEM_NULL("excluded_paths", ExcludedPaths)
                JSON_ITEM("id", Id)
                JSON_ITEM("path", Path)
                JSON_ITEM("tags", Tags)
                JSON_ITEM("url", Url)
            JSON_DEFINE_END
        };

        struct DynamicTagHeuristics
        {
            std::vector<JsonString> Countries;

            JSON_DEFINE
                JSON_ITEM("countries", Countries)
            JSON_DEFINE_END
        };

        struct DynamicTag
        {
            DynamicTagHeuristics Heuristics;
            std::vector<JsonString> Tags;

            JSON_DEFINE
                JSON_ITEM_NULL("heuristics", Heuristics)
                JSON_ITEM("tags", Tags)
            JSON_DEFINE_END
        };

        using Entitlement = Web::Json::Document;

        struct Configuration
        {
            std::vector<JsonString> AllowedHttpFallbackHostnames;
            JsonString BundlesUrl;
            bool DeleteForeignPaths;
            std::vector<Dependency> Dependencies;
            bool Disallow32BitWindows;
            std::vector<DynamicTag> DynamicTags;
            std::vector<Entitlement> Entitlements;
            std::vector<JsonString> ExcludedPaths;
            JsonString Id;
            bool LaunchableOnUpdateFail;
            Launcher Launcher;
            Locales LocaleData;
            Meta Metadata;
            JsonString PatchNotesUrl;
            JsonString PatchUrl;
            Regions RegionData;
            std::vector<SecondaryPatchline> SecondaryPatchlines;
            JsonString SeedUrl;
            std::vector<JsonString> Tags;
            Shards ValidShards;
            JsonString Version;

            JSON_DEFINE
                JSON_ITEM_NULL("allowed_http_fallback_hostnames", AllowedHttpFallbackHostnames)
                JSON_ITEM("bundles_url", BundlesUrl)
                JSON_ITEM_OPT("delete_foreign_paths", DeleteForeignPaths)
                JSON_ITEM_NULL("dependencies", Dependencies)
                JSON_ITEM_OPT("disallow_32bit_windows", Disallow32BitWindows)
                JSON_ITEM_OPT("dynamic_tags", DynamicTags)
                JSON_ITEM_NULL("entitlements", Entitlements)
                JSON_ITEM_NULL("excluded_paths", ExcludedPaths)
                JSON_ITEM("id", Id)
                JSON_ITEM("launchable_on_update_fail", LaunchableOnUpdateFail)
                JSON_ITEM_OPT("launcher", Launcher)
                JSON_ITEM_OPT("locale_data", LocaleData)
                JSON_ITEM("metadata", Metadata)
                JSON_ITEM("patch_notes_url", PatchNotesUrl)
                JSON_ITEM("patch_url", PatchUrl)
                JSON_ITEM_OPT("region_data", RegionData)
                JSON_ITEM_NULL("secondary_patchlines", SecondaryPatchlines)
                JSON_ITEM("seed_url", SeedUrl)
                JSON_ITEM("tags", Tags)
                JSON_ITEM_OPT("valid_shards", ValidShards)
                JSON_ITEM_OPT("version", Version)
            JSON_DEFINE_END
        };

        struct Platform
        {
            bool AutoPatch;
            std::vector<Configuration> Configurations;
            std::vector<Dependency> Dependencies;
            JsonString DeprecatedCloudfrontId;
            JsonString IconPath;
            JsonString InstallDir;
            std::vector<JsonString> RogueProcessChecklist;
            Launcher Launcher;

            JSON_DEFINE
                JSON_ITEM("auto_patch", AutoPatch)
                JSON_ITEM("configurations", Configurations)
                JSON_ITEM_NULL("dependencies", Dependencies)
                JSON_ITEM("deprecated_cloudfront_id", DeprecatedCloudfrontId)
                JSON_ITEM_OPT("icon_path", IconPath)
                JSON_ITEM_OPT("install_dir", InstallDir)
                JSON_ITEM_OPT("rogue_process_checklist", RogueProcessChecklist)
                JSON_ITEM_OPT("launcher", Launcher)
            JSON_DEFINE_END
        };

        Locales LocaleData;
        std::unordered_map<JsonString, Meta> Metadata;
        std::unordered_map<JsonString, Platform> Platforms;
        JsonString Version;

        JSON_DEFINE
            JSON_ITEM_OPT("locale_data", LocaleData)
            JSON_ITEM("metadata", Metadata)
            JSON_ITEM("platforms", Platforms)
            JSON_ITEM("version", Version)
        JSON_DEFINE_END
    };
}