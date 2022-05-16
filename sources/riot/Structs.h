#pragma once

#include <SourceStructs.h>

using namespace L4::Interface::Generic;
using namespace L4::Interface;

static constexpr Guid LanguageInfoStreamGuid { 0xD10EA487, 0x2E153BE9, 0xD71E342B, 0xB108B7DF };

using ChunkInfo = ChunkInfoGeneric;

struct FileStructure : public FileStructureGeneric
{
    uint64_t LanguageMask;
};

using DirectoryStructure = DirectoryStructureGeneric;

struct LanguageInfo
{
    StreamSpan<ExtraBlobStreamGuid, std::byte> Name;
    uint8_t LanguageId;
};