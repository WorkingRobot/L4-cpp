#pragma once

#ifdef _WIN32

#ifdef L4_EXPORT
#define L4_API __declspec(dllexport)
#else
#define L4_API __declspec(dllimport)
#endif

#else
#define L4_API __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#ifdef __clang__
#define L4_CLASS_API [[clang::lto_visibility_public]]
#else
#define L4_CLASS_API
#endif
#else
#define L4_CLASS_API __attribute__((visibility("default")))
#endif

#include <string_view>
#include <span>

extern "C" {
    L4_API void* InitializeSource(uint32_t Version);

    L4_API void UninitializeSource(void* Source);
}

namespace L4::Interface
{
    enum class SourceVersion : uint32_t
    {
        Unknown,
        Initial,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };

    enum class Error : uint16_t
    {
        Success,
        UnsupportedArchive,
        UnsupportedOperation,
        StreamElementSizeMismatch,
        StreamVersionMismatch,
        RecursiveDirectoryTree
    };

    struct Guid
    {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;

        auto operator<=>(const Guid&) const = default;
    };

    struct ArchiveTreeNode
    {
        std::u8string_view Name;
        uint64_t Created;
        uint64_t Modified;
        uint64_t Accessed;
        uint16_t Hidden : 1;
        uint16_t System : 1;
    };

    enum class FileReadStrategy : uint8_t
    {
        Unknown,
        Direct,
        Callback,
        IntervalDirect,
        IntervalCallback
    };

    struct ArchivePosition
    {
        uint32_t StreamIdx;
        uint64_t StreamOffset;
    };

    // [Start, End]
    template<class T>
    struct FileInterval
    {
        uint64_t Start;
        uint64_t End;
        T Data;
    };

    struct ArchiveTreeFile : public ArchiveTreeNode
    {
        uint64_t FileSize;
        FileReadStrategy ReadStrategy;
        union ReadStrategyData_
        {
            std::monostate Unknown;
            ArchivePosition Direct;
            void* Callback;
            std::span<FileInterval<ArchivePosition>> IntervalDirect;
            std::span<FileInterval<void*>> IntervalCallback;

            ReadStrategyData_() :
                Unknown() {};
        } ReadStrategyData;
    };

    struct ArchiveTreeDirectory : public ArchiveTreeNode
    {
        std::span<ArchiveTreeDirectory> Directories;
        std::span<ArchiveTreeFile> Files;
    };

    struct ArchiveTree
    {
        std::span<ArchiveTreeDirectory> Directories;
        std::span<ArchiveTreeFile> Files;
        std::string_view Timezone;
    };

    class L4_CLASS_API StreamFI
    {
    public:
        StreamFI() = default;
        virtual ~StreamFI() = default;

        StreamFI(const StreamFI&) = delete;
        StreamFI(StreamFI&&) = delete;
        StreamFI& operator=(const StreamFI&) = delete;
        StreamFI& operator=(StreamFI&&) = delete;

        virtual uint32_t GetId() const = 0;

        virtual Guid GetGuid() const = 0;

        virtual uint32_t GetVersion() const = 0;

        virtual uint32_t GetElementSize() const = 0;

        virtual size_t GetName(char8_t* Name, size_t NameSize) const = 0;

        virtual size_t GetContext(char* Context, size_t ContextSize) const = 0;

        virtual size_t GetSize() const = 0;

        virtual size_t ReadBytes(size_t Offset, char* Dst, size_t ByteCount) const = 0;
    };

    class L4_CLASS_API ArchiveFI
    {
    public:
        ArchiveFI() = default;
        virtual ~ArchiveFI() = default;

        ArchiveFI(const ArchiveFI&) = delete;
        ArchiveFI(ArchiveFI&&) = delete;
        ArchiveFI& operator=(const ArchiveFI&) = delete;
        ArchiveFI& operator=(ArchiveFI&&) = delete;

        virtual uint32_t GetSectorSize() const = 0;

        virtual size_t GetSourceName(char8_t* Name, size_t NameSize) const = 0;

        virtual size_t GetAppName(char8_t* Name, size_t NameSize) const = 0;

        virtual size_t GetSourceVersion(char8_t* Name, size_t NameSize) const = 0;

        virtual size_t GetAppVersion(char8_t* Name, size_t NameSize) const = 0;

        virtual uint32_t GetSourceVersionNumeric() const = 0;

        virtual uint32_t GetAppVersionNumeric() const = 0;

        virtual Guid GetSourceGuid() const = 0;

        virtual Guid GetAppGuid() const = 0;

        virtual uint32_t GetStreamCount() const = 0;

        virtual Error OpenStream(StreamFI** Stream, uint32_t StreamIdx) const = 0;

        virtual Error OpenStream(StreamFI** Stream, const Guid& Guid) const = 0;
    };

    class L4_CLASS_API Archive
    {
    public:
        Archive() = default;
        virtual ~Archive() = default;

        Archive(const Archive&) = delete;
        Archive(Archive&&) = delete;
        Archive& operator=(const Archive&) = delete;
        Archive& operator=(Archive&&) = delete;

        virtual const ArchiveTree& GetTree() const = 0;

        virtual Error ReadFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const = 0;
    };

    class L4_CLASS_API Source
    {
    public:
        static const SourceVersion Version = SourceVersion::Initial;

        Source() = default;
        virtual ~Source() = default;

        Source(const Source&) = delete;
        Source(Source&&) = delete;
        Source& operator=(const Source&) = delete;
        Source& operator=(Source&&) = delete;

        virtual Guid GetGuid() const = 0;

        virtual Error OpenArchive(Archive** Archive, const ArchiveFI* Interface) = 0;
    };

    static_assert(Source::Version == SourceVersion::Latest);
}