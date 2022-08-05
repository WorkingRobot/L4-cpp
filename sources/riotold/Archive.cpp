#include "Archive.h"

#include "Consts.h"

#include <source/Error.h>

namespace L4::Source::Riot
{
    Archive::Archive(IArchive::FIPtr Interface) :
        IArchive(Interface)
    {
        if (Interface->GetSourceGuid() != SourceGuid)
        {
            throw Error::UnsupportedArchive;
        }
        if (Interface->GetSourceVersionNumeric() != SourceVersionNumeric)
        {
            throw Error::UnsupportedArchive;
        }

        static const auto CreateStream = [this](const auto& t) {
            using T = typename std::decay_t<decltype(t)>::element_type;
            return CreateObject<T>(this->Interface->OpenStream(T::Guid));
        };
        StreamChunkInfo = CreateStream(StreamChunkInfo);
        StreamChunkData = CreateStream(StreamChunkData);
        StreamIdBlob = CreateStream(StreamIdBlob);
        StreamExtraBlob = CreateStream(StreamExtraBlob);
        StreamFileStructure = CreateStream(StreamFileStructure);
        StreamDirectoryStructure = CreateStream(StreamDirectoryStructure);
        StreamLanguageInfo = CreateStream(StreamLanguageInfo);

        CreateTree();
    }

    Ptr<const ArchiveTree> Archive::GetTree() const
    {
        return Ptr<const ArchiveTree>();
    }

    uint64_t Archive::ReadTreeFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const
    {
        throw Error::UnsupportedOperation;
    }

    void Archive::CreateTree()
    {
        if (Tree)
        {
            return;
        }

        TreeExtraBlob = std::make_unique<std::byte[]>(StreamExtraBlob->Size());
        StreamExtraBlob->Read(0, TreeExtraBlob.get(), StreamExtraBlob->Size());
        TreeFiles.reserve(StreamFileStructure->Size());
        TreeDirectories.reserve(StreamDirectoryStructure->Size());
        uint64_t IntervalCount = 0;
        for (auto File : *StreamFileStructure)
        {
            IntervalCount += File.Chunks.Size;
        }
        TreeIntervals.reserve(IntervalCount);

        auto RootDirectory = *(StreamDirectoryStructure->Begin());
        Tree = std::make_shared<ArchiveTree>(
            ArchiveTree {
                .Directories = CreateTreeDirectories(RootDirectory.Directories),
                .Files = CreateTreeFiles(RootDirectory.Files),
                .Timezone = "UTC" });
    }

    std::span<ArchiveTreeDirectory> Archive::CreateTreeDirectories(IdStreamSpan<DirectoryStructureStreamGuid> Directories)
    {
        if (TreeDirectories.size() + Directories.Size > TreeDirectories.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<ArchiveTreeDirectory> Ret = { TreeDirectories.data() + TreeDirectories.size(), Directories.Size };
        TreeDirectories.resize(TreeDirectories.size() + Directories.Size);
        uint64_t Idx = 0;
        for (auto Itr = StreamIdBlob->IdBegin(StreamDirectoryStructure) + Directories.Offset,
                  End = Itr + Directories.Size;
             Itr != End;
             ++Itr)
        {
            auto Directory = *Itr;
            ArchiveTreeDirectory TreeDirectory {};
            TreeDirectory.Name = std::u8string_view((char8_t*)TreeExtraBlob.get() + Directory.Name.Offset, Directory.Name.Size);
            TreeDirectory.Created = TreeDirectory.Modified = TreeDirectory.Accessed = -1;
            TreeDirectory.Hidden = false;
            TreeDirectory.System = false;
            TreeDirectory.Directories = CreateTreeDirectories(Directory.Directories);
            TreeDirectory.Files = CreateTreeFiles(Directory.Files);
            Ret[Idx++] = TreeDirectory;
        }
        return Ret;
    }

    std::span<ArchiveTreeFile> Archive::CreateTreeFiles(IdStreamSpan<FileStructureStreamGuid> Files)
    {
        if (TreeFiles.size() + Files.Size > TreeFiles.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<ArchiveTreeFile> Ret = { TreeFiles.data() + TreeFiles.size(), Files.Size };
        TreeFiles.resize(TreeFiles.size() + Files.Size);
        uint64_t Idx = 0;
        for (auto Itr = StreamIdBlob->IdBegin(StreamFileStructure) + Files.Offset,
                  End = Itr + Files.Size;
             Itr != End;
             ++Itr)
        {
            auto File = *Itr;
            ArchiveTreeFile TreeFile {};
            TreeFile.Name = std::u8string_view((char8_t*)TreeExtraBlob.get() + File.Name.Offset, File.Name.Size);
            TreeFile.Created = TreeFile.Modified = TreeFile.Accessed = -1;
            TreeFile.Hidden = false;
            TreeFile.System = false;
            TreeFile.FileSize = File.FileSize;
            TreeFile.ReadStrategy = FileReadStrategy::IntervalDirect;
            TreeFile.ReadStrategyData.IntervalDirect = CreateTreeIntervals(File.Chunks);
            Ret[Idx++] = TreeFile;
        }
        return Ret;
    }

    std::span<FileInterval<ArchivePosition>> Archive::CreateTreeIntervals(IdStreamSpan<ChunkInfoStreamGuid> Chunks)
    {
        if (TreeIntervals.size() + Chunks.Size > TreeIntervals.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<FileInterval<ArchivePosition>> Ret = { TreeIntervals.data() + TreeFiles.size(), Chunks.Size };
        TreeIntervals.resize(TreeIntervals.size() + Chunks.Size);
        uint64_t Idx = 0;
        uint64_t FileOffset = 0;
        for (auto Itr = StreamIdBlob->IdBegin(StreamChunkInfo) + Chunks.Offset,
                  End = Itr + Chunks.Size;
             Itr != End;
             ++Itr)
        {
            auto Chunk = *Itr;
            FileInterval<ArchivePosition> TreeInterval {};
            TreeInterval.Start = FileOffset,
            TreeInterval.End = Chunk.Data.Size - 1,
            TreeInterval.Context = ArchivePosition { .StreamIdx = StreamChunkData->GetId(), .StreamOffset = Chunk.Data.Offset };
            FileOffset += Chunk.Data.Size;

            Ret[Idx++] = TreeInterval;
        }
        return Ret;
    }
}