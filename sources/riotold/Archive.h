#pragma once

#include <source/Archive.h>
#include "Stream.h"
#include "Structs.h"

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    class Archive : public IArchive
    {
        template <class T, class... ArgTs>
        friend OwningPtr<T> L4::Interface::CreateObject(ArgTs&&... Args);

    public:
        Archive(IArchive::FIPtr Interface);

        Ptr<const ArchiveTree> GetTree() const override;

        uint64_t ReadTreeFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const override;

    private:
        void CreateTree();

        std::span<ArchiveTreeDirectory> CreateTreeDirectories(IdStreamSpan<DirectoryStructureStreamGuid> Directories);

        std::span<ArchiveTreeFile> CreateTreeFiles(IdStreamSpan<FileStructureStreamGuid> Files);

        std::span<FileInterval<ArchivePosition>> CreateTreeIntervals(IdStreamSpan<ChunkInfoStreamGuid> Chunks);

        OwningPtr<Stream<ChunkInfoStreamGuid, ChunkInfo, 0>> StreamChunkInfo;
        OwningPtr<Stream<ChunkDataStreamGuid, std::byte, 0>> StreamChunkData;
        OwningPtr<Stream<IdBlobStreamGuid, uint64_t, 0>> StreamIdBlob;
        OwningPtr<Stream<ExtraBlobStreamGuid, std::byte, 0>> StreamExtraBlob;
        OwningPtr<Stream<FileStructureStreamGuid, FileStructure, 0>> StreamFileStructure;
        OwningPtr<Stream<DirectoryStructureStreamGuid, DirectoryStructure, 0>> StreamDirectoryStructure;
        OwningPtr<Stream<LanguageInfoStreamGuid, LanguageInfo, 0>> StreamLanguageInfo;

        OwningPtr<ArchiveTree> Tree;
        std::vector<ArchiveTreeFile> TreeFiles;
        std::vector<ArchiveTreeDirectory> TreeDirectories;
        std::vector<FileInterval<ArchivePosition>> TreeIntervals;
        std::unique_ptr<std::byte[]> TreeExtraBlob;
    };
}