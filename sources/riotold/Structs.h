#pragma once

#include <source/ArchiveStructs.h>

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    static constexpr Guid ChunkInfoStreamGuid = Generic::ChunkInfoStreamGuid;
    static constexpr Guid ChunkDataStreamGuid = Generic::ChunkDataStreamGuid;
    static constexpr Guid IdBlobStreamGuid = Generic::IdBlobStreamGuid;
    static constexpr Guid ExtraBlobStreamGuid = Generic::ExtraBlobStreamGuid;
    static constexpr Guid FileStructureStreamGuid = Generic::FileStructureStreamGuid;
    static constexpr Guid DirectoryStructureStreamGuid = Generic::DirectoryStructureStreamGuid;
    static constexpr Guid LanguageInfoStreamGuid { 0xD10EA487, 0x2E153BE9, 0xD71E342B, 0xB108B7DF };

    using Generic::StreamSpan;
    using Generic::IdStreamSpan;
    using Generic::ChunkInfo;
    using Generic::DirectoryStructure;

    struct FileStructure : public Generic::FileStructure
    {
        uint64_t LanguageMask;
    };

    struct LanguageInfo
    {
        StreamSpan<ExtraBlobStreamGuid, std::byte> Name;
        uint8_t LanguageId;
    };
}