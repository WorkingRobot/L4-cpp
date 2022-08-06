#pragma once

#include <filesystem>
#include "SourceStructs.h"
#include "SourceLibrary.h"

namespace L4
{
    class SourceManager
    {
    public:
        SourceManager() = default;

        void Load(const std::filesystem::path& FilePath);

    private:
        struct InterfaceWrapper
        {
            // Archive
            static void ArchiveGetIdentity(Source::Archive Archive, Source::AppIdentity* OutIdentity);
            static void ArchiveSetIdentity(Source::Archive Archive, const Source::AppIdentity* NewIdentity);
            static uint32_t ArchiveGetStreamCount(Source::Archive Archive);
            static uint32_t ArchiveGetSectorSize(Source::Archive Archive);
            static uint32_t ArchiveGetStreamIdxFromId(Source::Archive Archive, const Source::String* Id);
            static void ArchiveOpenStreamRead(Source::Archive Archive, uint32_t StreamIdx, Source::Stream* OutStream);
            static void ArchiveOpenStreamWrite(Source::Archive Archive, uint32_t StreamIdx, Source::Stream* OutStream);
            static void ArchiveCloseStream(Source::Archive Archive, Source::Stream Stream);

            // Stream
            static void ArchiveStreamGetIdentity(Source::Stream Stream, Source::StreamIdentity* OutIdentity);
            static void ArchiveStreamSetIdentity(Source::Stream Stream, const Source::StreamIdentity* NewIdentity);
            static uint32_t ArchiveStreamGetElementSize(Source::Stream Stream);
            static void ArchiveStreamSetElementSize(Source::Stream Stream, uint32_t NewElementSize);
            static void ArchiveStreamReadContext(Source::Stream Stream, void* Dst, uint32_t Size);
            static void ArchiveStreamWriteContext(Source::Stream Stream, const void* Src, uint32_t Size);
            static uint64_t ArchiveStreamGetCapacity(Source::Stream Stream);
            static uint64_t ArchiveStreamGetSize(Source::Stream Stream);
            static void ArchiveStreamReserveCapacity(Source::Stream Stream, uint64_t NewCapacity);
            static void ArchiveStreamShrinkToFit(Source::Stream Stream);
            static void ArchiveStreamResize(Source::Stream Stream, uint64_t NewSize);
            static uint64_t ArchiveStreamReadBytes(Source::Stream Stream, void* Dst, uint64_t Size, uint64_t Offset);
            static uint64_t ArchiveStreamWriteBytes(Source::Stream Stream, const void* Src, uint64_t Size, uint64_t Offset);

            // Update
            static Source::UpdateState UpdateGetState(Source::Update Update);
            static void UpdateOnStart(Source::Update Update, const Source::UpdateStartInfo* StartInfo);
            static void UpdateOnProgress(Source::Update Update, const Source::UpdateProgressInfo* ProgressInfo);
            static void UpdateOnPieceUpdate(Source::Update Update, uint64_t Id, Source::UpdatePieceStatus NewStatus);
            static void UpdateOnFinalize(Source::Update Update);
            static void UpdateOnComplete(Source::Update Update);
        };

        static const Source::L4Interface& GetInterface() noexcept;

        const SourceLibrary* GetSource(std::u8string_view Id) const;

        std::vector<SourceLibrary> Sources;
    };
}