#pragma once

#include "SourceInterface.h"

namespace L4::Interface::Generic
{
    static constexpr Guid ChunkInfoStreamGuid { 0x836BCFA4, 0xEB58FBD7, 0x549C2BD6, 0x9B4FB439 };
    static constexpr Guid ChunkDataStreamGuid { 0xE202050E, 0xCE6C20E9, 0x6971ACFE, 0xFB45F147 };
    static constexpr Guid IdBlobStreamGuid { 0xB74FA109, 0xFDB12629, 0xECE6D2D1, 0xB085D176 };
    static constexpr Guid ExtraBlobStreamGuid { 0x196150C3, 0xE6B8D18C, 0x45C721A5, 0xD0B3D896 };
    static constexpr Guid FileStructureStreamGuid { 0x5DC35480, 0x543AE5C1, 0xE562ADE8, 0x6A2A7D81 };
    static constexpr Guid DirectoryStructureStreamGuid { 0x25ADA7F9, 0xDE0EBF7A, 0x13D6D715, 0x9CE79FA5 };

    template <Guid StreamId, class T>
    struct StreamSpan
    {
        static constexpr Guid Stream = StreamId;

        uint64_t Offset;
        uint64_t Size;
    };

    template <Guid SubStreamId>
    struct IdStreamSpan
    {
        static constexpr Guid Stream = IdBlobStreamGuid;
        static constexpr Guid SubStream = SubStreamId;

        uint64_t Offset;
        uint64_t Size;
    };

    struct ChunkInfoGeneric
    {
        StreamSpan<ChunkDataStreamGuid, std::byte> Data;
    };

    struct FileStructureGeneric
    {
        StreamSpan<ExtraBlobStreamGuid, std::byte> Name;
        uint64_t FileSize;
        IdStreamSpan<ChunkInfoStreamGuid> Chunks;
    };

    struct DirectoryStructureGeneric
    {
        StreamSpan<ExtraBlobStreamGuid, std::byte> Name;
        IdStreamSpan<DirectoryStructureStreamGuid> Directories;
        IdStreamSpan<FileStructureStreamGuid> Files;
    };
}