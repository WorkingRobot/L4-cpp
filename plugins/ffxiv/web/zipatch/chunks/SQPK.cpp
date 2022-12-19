#include "SQPK.h"

#include "../Endian.h"

#include "sqpk/Add.h"
#include "sqpk/Delete.h"
#include "sqpk/Expand.h"
#include "sqpk/File.h"
#include "sqpk/Header.h"
#include "sqpk/Target.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    std::unique_ptr<PatchChunk> SQPK::Read(Stream& Stream)
    {
        uint32_t ChunkSize = Stream.Read<BE<uint32_t>>(); // Same as the external size field

        SQPKType ChunkType = Stream.Read<SQPKType>();
        std::unique_ptr<PatchChunk> Chunk;
        switch (ChunkType)
        {
        case SQPKType::Add:
            Chunk = std::make_unique<Chunks::SQPKAdd>();
            Stream >> (Chunks::SQPKAdd&)*Chunk;
            break;
        case SQPKType::Delete:
            Chunk = std::make_unique<Chunks::SQPKDelete>();
            Stream >> (Chunks::SQPKDelete&)*Chunk;
            break;
        case SQPKType::Expand:
            Chunk = std::make_unique<Chunks::SQPKExpand>();
            Stream >> (Chunks::SQPKExpand&)*Chunk;
            break;
        case SQPKType::File:
            Chunk = Chunks::SQPKFile::Read(Stream);
            break;
        case SQPKType::Header:
            Chunk = std::make_unique<Chunks::SQPKHeader>();
            Stream >> (Chunks::SQPKHeader&)*Chunk;
            break;
        case SQPKType::Target:
            Chunk = std::make_unique<Chunks::SQPKTarget>();
            Stream >> (Chunks::SQPKTarget&)*Chunk;
            break;
        case SQPKType::Index:
        case SQPKType::Patch:
            // throw std::runtime_error("Unsupported chunk type");
            break;
        default:
            throw std::runtime_error("Invalid SQPK chunk type");
        }
        return Chunk;
    }
}