#include "PatchChunk.h"

#include "chunks/ADIR.h"
#include "chunks/APLY.h"
#include "chunks/DELD.h"
#include "chunks/EOF_.h"
#include "chunks/FHDR.h"
#include "chunks/SQPK.h"
#include "Crc32Stream.h"
#include "Endian.h"
#include "SubStream.h"

#include <streams/Stream.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    std::unique_ptr<PatchChunk> PatchChunk::Read(Stream& Stream)
    {
        uint32_t ChunkSize = Stream.Read<BE<uint32_t>>();

        Crc32Stream CrcStream(Stream);

        auto ChunkType = CrcStream.Read<PatchChunkType>();

        SubStream ChunkStream(CrcStream, CrcStream.Tell(), ChunkSize);
        std::unique_ptr<PatchChunk> Chunk;
        switch (ChunkType)
        {
        case PatchChunkType::FHDR:
            Chunk = std::make_unique<Chunks::FHDR>();
            ChunkStream >> (Chunks::FHDR&)*Chunk;
            break;
        case PatchChunkType::APLY:
            Chunk = std::make_unique<Chunks::APLY>();
            ChunkStream >> (Chunks::APLY&)*Chunk;
            break;
        case PatchChunkType::ADIR:
            Chunk = std::make_unique<Chunks::ADIR>();
            ChunkStream >> (Chunks::ADIR&)*Chunk;
            break;
        case PatchChunkType::DELD:
            Chunk = std::make_unique<Chunks::DELD>();
            ChunkStream >> (Chunks::DELD&)*Chunk;
            break;
        case PatchChunkType::SQPK:
            Chunk = Chunks::SQPK::Read(ChunkStream);
            break;
        case PatchChunkType::EOF_:
            Chunk = std::make_unique<Chunks::EOF_>();
            ChunkStream >> (Chunks::EOF_&)*Chunk;
            break;
        case PatchChunkType::APFS:
        case PatchChunkType::XXXX:
            throw std::runtime_error("Unsupported chunk type");
        default:
            throw std::runtime_error("Invalid chunk type");
        }

        if (auto BytesLeft = ChunkStream.Size() - ChunkStream.Tell(); BytesLeft)
        {
            printf("Skipping %zu bytes\n", BytesLeft);
            ChunkStream.Skip(BytesLeft);
            // throw std::runtime_error("Invalid chunk size");
        }

        uint32_t Checksum = CrcStream.GetChecksum();
        uint32_t ExpectedChecksum = Stream.Read<BE<uint32_t>>();
        if (Checksum != ExpectedChecksum)
        {
            throw std::runtime_error("Invalid chunk checksum");
        }

        return Chunk;
    }
}