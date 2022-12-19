#include "File.h"

#include "../../Endian.h"

#include "file/Add.h"
#include "file/Delete.h"
#include "file/Mkdir.h"
#include "file/Remove.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    enum class SQPKFileType : uint8_t
    {
        Add = 'A',
        Remove = 'R',
        Delete = 'D',
        Mkdir = 'M',
    };

    std::unique_ptr<PatchChunk> Chunks::SQPKFile::Read(Stream& Stream)
    {
        SQPKFileType FileType = Stream.Read<SQPKFileType>();
        Stream.Skip(2); // Alignment

        std::unique_ptr<PatchChunk> Chunk;
        switch (FileType)
        {
        case SQPKFileType::Add:
            Chunk = std::make_unique<Chunks::SQPKFileAdd>();
            Stream >> (Chunks::SQPKFileAdd&)*Chunk;
            break;
        case SQPKFileType::Remove:
            Chunk = std::make_unique<Chunks::SQPKFileRemove>();
            Stream >> (Chunks::SQPKFileRemove&)*Chunk;
            break;
        case SQPKFileType::Delete:
            Chunk = std::make_unique<Chunks::SQPKFileDelete>();
            Stream >> (Chunks::SQPKFileDelete&)*Chunk;
            break;
        case SQPKFileType::Mkdir:
            Chunk = std::make_unique<Chunks::SQPKFileMkdir>();
            Stream >> (Chunks::SQPKFileMkdir&)*Chunk;
            break;
        default:
            throw std::runtime_error("Invalid SQPK File chunk type");
        }
        return Chunk;
    }

    void SQPKFile::Deserialize(Stream& Stream)
    {
        FileOffset = Stream.Read<BE<uint64_t>>();
        FileSize = Stream.Read<BE<uint64_t>>();

        uint32_t PathLength = Stream.Read<BE<uint32_t>>() - 1; // Removes null terminator

        ExpansionId = Stream.Read<BE<uint16_t>>();
        Stream.Skip(2); // Alignment

        FileName.resize_and_overwrite(PathLength, [&Stream](char* Buffer, size_t Count) {
            Stream.ReadBytes(std::as_writable_bytes(std::span(Buffer, Count)));
            return Count;
        });
        Stream.Skip(1); // Null terminator
    }
}