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
        UnsupportedArchive
    };

    struct Guid
    {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
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
        Callback,
        Intervals
    };

    struct ArchivePosition
    {
        uint32_t StreamIdx;
        uint64_t StreamOffset;
    };

    // [Start, End]
    struct FileInterval
    {
        uint64_t Start;
        uint64_t End;
        ArchivePosition Position;
    };

    struct ArchiveTreeFile : public ArchiveTreeNode
    {
        union
        {
            struct
            {
            
            } Unknown;
            struct
            {
                void* Context;
            } Callback;
            struct
            {
                std::span<FileInterval> Intervals;
            } Intervals;
        } ReadStrategyData;
        FileReadStrategy ReadStrategy;
        uint64_t FileSize;
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

    class L4_CLASS_API StreamFI_1
    {
    public:
        virtual ~StreamFI_1() = default;

        virtual Guid GetGuid() const = 0;

        virtual uint32_t GetVersion() const = 0;

        virtual uint32_t GetElementSize() const = 0;

        virtual size_t GetName(char8_t* Name, size_t NameSize) const = 0;

        virtual size_t GetContext(char* Context, size_t ContextSize) const = 0;

        virtual size_t GetSize() const = 0;

        virtual size_t ReadBytes(size_t Offset, char* Dst, size_t ByteCount) const = 0;

        virtual size_t Read(size_t Offset, void* Dst, size_t Count) const = 0;
    };

    class L4_CLASS_API ArchiveFI_1
    {
    public:
        using StreamFI = StreamFI_1;

        virtual ~ArchiveFI_1() = default;

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

        virtual Error CloseStream(StreamFI* Stream) const = 0;
    };

    class L4_CLASS_API Archive_1
    {
    public:
        using ArchiveFI = ArchiveFI_1;

        virtual ~Archive_1() = default;

        virtual const ArchiveTree& GetTree() const = 0;

        virtual Error ReadFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const = 0;
    };

    class L4_CLASS_API Source_1
    {
    public:
        static const SourceVersion Version = SourceVersion::Initial;
        using Archive = Archive_1;
        using ArchiveFI = ArchiveFI_1;

        virtual ~Source_1() = default;

        virtual Guid GetGuid() const = 0;

        virtual Error OpenArchive(Archive** Archive, ArchiveFI* Interface) = 0;

        virtual Error CloseArchive(Archive* Archive) = 0;
    };

    using Source = Source_1;
    static_assert(Source::Version == SourceVersion::Latest);
}