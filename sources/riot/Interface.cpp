#define L4_EXPORT
#include <SourceInterface.h>
#include "Structs.h"
#include <memory>
#include <vector>

using namespace L4::Interface;

static constexpr Guid RiotGuid { 0x42D44220, 0x572E6186, 0x7B14E7DC, 0x1C92ECBF };
static constexpr uint32_t RiotVersion = 0;

template<class T>
class StreamIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    constexpr StreamIterator() noexcept = default;

    constexpr explicit StreamIterator(const StreamFI& Interface, uint64_t Position = 0) noexcept :
        Interface(&Interface), Position(Position), End(Interface.GetSize() / sizeof(T))
    {
        
    }

    template<Guid StreamId>
    constexpr explicit StreamIterator(const StreamFI& Interface, StreamSpan<StreamId, T> Span) noexcept :
        Interface(&Interface), Position(Span.Offset), End(Span.Offset + Span.Size)
    {
    }

    /*[[nodiscard]] constexpr reference operator*() const noexcept
    {
        return *GetPtr();
    }

    [[nodiscard]] constexpr pointer operator->() const noexcept
    {
        return GetPtr();
    }*/

    [[nodiscard]] constexpr value_type operator*() const noexcept
    {
        T Ret;
        Interface->ReadBytes(Position * sizeof(T), (char*)&Ret, sizeof(T));
        return Ret;
    }

    constexpr StreamIterator& operator++() noexcept
    {
        ++Position;
        return *this;
    }

    constexpr StreamIterator operator++(int) noexcept
    {
        StreamIterator Tmp = *this;
        ++Position;
        return Tmp;
    }

    constexpr StreamIterator& operator--() noexcept
    {
        --Position;
        return *this;
    }

    constexpr StreamIterator operator--(int) noexcept
    {
        StreamIterator Tmp = *this;
        --Position;
        return Tmp;
    }

    constexpr StreamIterator& operator+=(const ptrdiff_t Offset) noexcept
    {
        Position += Offset;
        return *this;
    }

    [[nodiscard]] constexpr StreamIterator operator+(const ptrdiff_t Offset) const noexcept
    {
        StreamIterator Tmp = *this;
        Tmp += Offset;
        return Tmp;
    }

    constexpr StreamIterator& operator-=(const ptrdiff_t Offset) noexcept
    {
        Position -= Offset;
        return *this;
    }

    [[nodiscard]] constexpr StreamIterator operator-(const ptrdiff_t Offset) const noexcept
    {
        StreamIterator Tmp = *this;
        Tmp -= Offset;
        return Tmp;
    }

    [[nodiscard]] constexpr ptrdiff_t operator-(const StreamIterator& _Right) const noexcept
    {
        return Position - _Right.Position;
    }

    [[nodiscard]] constexpr reference operator[](const ptrdiff_t Offset) const noexcept
    {
        return *(*this + Offset);
    }

    [[nodiscard]] constexpr bool operator==(const StreamIterator& _Right) const noexcept
    {
        if (AtEnd() != _Right.AtEnd())
        {
            return false;
        }
        if (!AtEnd())
        {
            return Position == _Right.Position;
        }
        return true;
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const StreamIterator& _Right) const noexcept
    {
        return Position <=> _Right.Position;
    }

private:
    [[nodiscard]] constexpr bool AtEnd() const noexcept
    {
        return Position == End;
    }

    const StreamFI* Interface;
    uint64_t Position;
    uint64_t End;
};

template <class T>
[[nodiscard]] inline StreamIterator<T> begin(StreamIterator<T> _Iter) noexcept
{
    return _Iter;
}

template <class T>
[[nodiscard]] inline StreamIterator<T> end(StreamIterator<T>) noexcept
{
    return {};
}

template <class T>
class IdStreamIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    constexpr IdStreamIterator() noexcept = default;

    template <Guid StreamId>
    constexpr explicit IdStreamIterator(const StreamFI& IdInterface, const StreamFI& SubInterface, IdStreamSpan<StreamId> Span) noexcept :
        IdInterface(&IdInterface), SubInterface(&SubInterface), Position(Span.Offset), End(Span.Offset + Span.Size)
    {
    }

    [[nodiscard]] constexpr value_type operator*() const noexcept
    {
        uint32_t RetId;
        IdInterface->ReadBytes(Position * sizeof(uint32_t), (char*)&RetId, sizeof(uint32_t));
        T Ret;
        SubInterface->ReadBytes(RetId * sizeof(T), (char*)&Ret, sizeof(T));
        return Ret;
    }

    constexpr IdStreamIterator& operator++() noexcept
    {
        ++Position;
        return *this;
    }

    constexpr IdStreamIterator operator++(int) noexcept
    {
        IdStreamIterator Tmp = *this;
        ++Position;
        return Tmp;
    }

    constexpr IdStreamIterator& operator--() noexcept
    {
        --Position;
        return *this;
    }

    constexpr IdStreamIterator operator--(int) noexcept
    {
        IdStreamIterator Tmp = *this;
        --Position;
        return Tmp;
    }

    constexpr IdStreamIterator& operator+=(const ptrdiff_t Offset) noexcept
    {
        Position += Offset;
        return *this;
    }

    [[nodiscard]] constexpr IdStreamIterator operator+(const ptrdiff_t Offset) const noexcept
    {
        IdStreamIterator Tmp = *this;
        Tmp += Offset;
        return Tmp;
    }

    constexpr IdStreamIterator& operator-=(const ptrdiff_t Offset) noexcept
    {
        Position -= Offset;
        return *this;
    }

    [[nodiscard]] constexpr IdStreamIterator operator-(const ptrdiff_t Offset) const noexcept
    {
        IdStreamIterator Tmp = *this;
        Tmp -= Offset;
        return Tmp;
    }

    [[nodiscard]] constexpr ptrdiff_t operator-(const IdStreamIterator& _Right) const noexcept
    {
        return Position - _Right.Position;
    }

    [[nodiscard]] constexpr reference operator[](const ptrdiff_t Offset) const noexcept
    {
        return *(*this + Offset);
    }

    [[nodiscard]] constexpr bool operator==(const IdStreamIterator& _Right) const noexcept
    {
        if (AtEnd() != _Right.AtEnd())
        {
            return false;
        }
        if (!AtEnd())
        {
            return Position == _Right.Position;
        }
        return true;
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const IdStreamIterator& _Right) const noexcept
    {
        return Position <=> _Right.Position;
    }

private:
    [[nodiscard]] constexpr bool AtEnd() const noexcept
    {
        return Position == End;
    }

    const StreamFI* IdInterface;
    const StreamFI* SubInterface;
    uint64_t Position;
    uint64_t End;
};

template <class T>
[[nodiscard]] inline IdStreamIterator<T> begin(IdStreamIterator<T> _Iter) noexcept
{
    return _Iter;
}

template <class T>
[[nodiscard]] inline IdStreamIterator<T> end(IdStreamIterator<T>) noexcept
{
    return {};
}

template<Guid Guid, class T, uint32_t Version = T::Version>
class Stream
{
public:
    Stream() = default;

    Stream(const ArchiveFI& ArchiveInterface)
    {
        if (auto Error = ArchiveInterface.OpenStream((StreamFI**)&Interface, Guid); Error != Error::Success)
        {
            throw Error;
        }
        if (Interface->GetElementSize() != sizeof(T))
        {
            throw Error::StreamElementSizeMismatch;
        }
        if (Interface->GetVersion() != Version)
        {
            throw Error::StreamVersionMismatch;
        }
    }

    ~Stream()
    {
        delete Interface;
    }

    const StreamFI& GetInterface() const noexcept
    {
        return *Interface;
    }

    uint32_t GetId() const
    {
        return Interface->GetId();
    }

    uint64_t Size() const
    {
        return Interface->GetSize() / sizeof(T);
    }

    std::unique_ptr<T[]> Read() const
    {
        auto BufferSize = Size();
        auto Buffer = std::make_unique<T[]>(BufferSize);
        Interface->ReadBytes(0, (char*)Buffer.get(), BufferSize * sizeof(T));
        return Buffer;
    }

    StreamIterator<T> Begin() const
    {
        return StreamIterator<T> { *Interface, 0 };
    }

    StreamIterator<T> End() const
    {
        return StreamIterator<T> { *Interface, Size() };
    }

    auto begin() const
    {
        return Begin();
    }

    auto end() const
    {
        return End();
    }

private:
    const StreamFI* Interface;
};

using StreamChunkInfo = Stream<ChunkInfoStreamGuid, ChunkInfo, 0>;
using StreamChunkData = Stream<ChunkDataStreamGuid, std::byte, 0>;
using StreamIdBlob = Stream<IdBlobStreamGuid, uint32_t, 0>;
using StreamExtraBlob = Stream<ExtraBlobStreamGuid, std::byte, 0>;
using StreamFileStructure = Stream<FileStructureStreamGuid, FileStructure, 0>;
using StreamDirectoryStructure = Stream<DirectoryStructureStreamGuid, DirectoryStructure, 0>;
using StreamLanguageInfo = Stream<LanguageInfoStreamGuid, LanguageInfo, 0>;

class ArchiveRiot final : public Archive
{
public:
    ArchiveRiot(const ArchiveFI& InterfaceRef) :
        Interface(&InterfaceRef)
    {
        if (Interface->GetSourceGuid() != RiotGuid)
        {
            throw Error::UnsupportedArchive;
        }
        if (Interface->GetSourceVersionNumeric() != RiotVersion)
        {
            throw Error::UnsupportedArchive;
        }
        
        ChunkInfo = StreamChunkInfo(*Interface);
        ChunkData = StreamChunkData(*Interface);
        IdBlob = StreamIdBlob(*Interface);
        ExtraBlob = StreamExtraBlob(*Interface);
        FileStructure = StreamFileStructure(*Interface);
        DirectoryStructure = StreamDirectoryStructure(*Interface);
        LanguageInfo = StreamLanguageInfo(*Interface);

        CreateTree();
    }

    ~ArchiveRiot() final
    {
        delete Interface;
    }

    const ArchiveTree& GetTree() const final
    {
        return Tree;
    }

    Error ReadFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const final
    {
        return Error::UnsupportedOperation;
    }

private:
    void CreateTree()
    {
        TreeExtraBlob = ExtraBlob.Read();
        TreeFiles.reserve(FileStructure.Size());
        TreeDirectories.reserve(DirectoryStructure.Size());
        uint64_t IntervalCount = 0;
        for (auto File : FileStructure)
        {
            IntervalCount += File.Chunks.Size;
        }
        TreeIntervals.reserve(IntervalCount);

        auto RootDirectory = *DirectoryStructure.Begin();
        Tree = {
            .Directories = CreateTreeDirectories(RootDirectory.Directories),
            .Files = CreateTreeFiles(RootDirectory.Files),
            .Timezone = "UTC"
        };
    }

    std::span<ArchiveTreeDirectory> CreateTreeDirectories(IdStreamSpan<DirectoryStructureStreamGuid> Directories)
    {
        if (TreeDirectories.size() + Directories.Size > TreeDirectories.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<ArchiveTreeDirectory> Ret = { TreeDirectories.data() + TreeDirectories.size(), Directories.Size };
        TreeDirectories.resize(TreeDirectories.size() + Directories.Size);
        uint64_t Idx = 0;
        for (auto Directory : IdStreamIterator<::DirectoryStructure>(IdBlob.GetInterface(), DirectoryStructure.GetInterface(), Directories))
        {
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

    std::span<ArchiveTreeFile> CreateTreeFiles(IdStreamSpan<FileStructureStreamGuid> Files)
    {
        if (TreeFiles.size() + Files.Size > TreeFiles.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<ArchiveTreeFile> Ret = { TreeFiles.data() + TreeFiles.size(), Files.Size };
        TreeFiles.resize(TreeFiles.size() + Files.Size);
        uint64_t Idx = 0;
        for (auto File : IdStreamIterator<::FileStructure>(IdBlob.GetInterface(), FileStructure.GetInterface(), Files))
        {
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

    std::span<FileInterval<ArchivePosition>> CreateTreeIntervals(IdStreamSpan<ChunkInfoStreamGuid> Chunks)
    {
        if (TreeIntervals.size() + Chunks.Size > TreeIntervals.capacity()) [[unlikely]]
        {
            throw Error::RecursiveDirectoryTree;
        }

        std::span<FileInterval<ArchivePosition>> Ret = { TreeIntervals.data() + TreeFiles.size(), Chunks.Size };
        TreeIntervals.resize(TreeIntervals.size() + Chunks.Size);
        uint64_t Idx = 0;
        uint64_t FileOffset = 0;
        for (auto Chunk : IdStreamIterator<::ChunkInfo>(IdBlob.GetInterface(), ChunkInfo.GetInterface(), Chunks))
        {
            FileInterval<ArchivePosition> TreeInterval {};
            TreeInterval.Start = FileOffset,
            TreeInterval.End = Chunk.Data.Size - 1,
            TreeInterval.Data = ArchivePosition { .StreamIdx = ChunkData.GetId(), .StreamOffset = Chunk.Data.Offset };
            FileOffset += Chunk.Data.Size;

            Ret[Idx++] = TreeInterval;
        }
        return Ret;
    }

    const ArchiveFI* Interface;

    StreamChunkInfo ChunkInfo;
    StreamChunkData ChunkData;
    StreamIdBlob IdBlob;
    StreamExtraBlob ExtraBlob;
    StreamFileStructure FileStructure;
    StreamDirectoryStructure DirectoryStructure;
    StreamLanguageInfo LanguageInfo;

    ArchiveTree Tree;
    std::vector<ArchiveTreeFile> TreeFiles;
    std::vector<ArchiveTreeDirectory> TreeDirectories;
    std::vector<FileInterval<ArchivePosition>> TreeIntervals;
    std::unique_ptr<std::byte[]> TreeExtraBlob;
};

class SourceRiot final : public Source
{
public:
    Guid GetGuid() const final
    {
        return RiotGuid;
    }

    Error OpenArchive(Archive** Archive, const ArchiveFI* Interface) final
    {
        try
        {
            *Archive = new ArchiveRiot(*Interface);
        }
        catch (const Error& Error)
        {
            return Error;
        }
        return Error::Success;
    }
};

extern "C" L4_API void* InitializeSource(uint32_t Version)
{
    if (Version != (uint32_t)SourceRiot::Version)
    {
        return nullptr;
    }
    return new SourceRiot;
}

extern "C" L4_API void UninitializeSource(void* Source)
{
    delete dynamic_cast<SourceRiot*>(reinterpret_cast<L4::Interface::Source*>(Source));
}