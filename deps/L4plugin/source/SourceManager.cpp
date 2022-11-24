#include "SourceManager.h"

#include "utils/Config.h"

namespace L4
{
    void SourceManager::Load(const std::filesystem::path& FilePath)
    {
        SourceLibrary Source(FilePath);
        if (!Source.Initialize(GetInterface()))
        {
            return;
        }
        if (auto ExistingSource = GetSource(DeserializeString(Source.GetIdentity().Id)); ExistingSource != nullptr)
        {
            return;
        }
        Sources.emplace_back(std::move(Source));
    }

    const Source::L4Interface& SourceManager::GetInterface() noexcept
    {
        static const Source::L4Interface Interface {
            .Identity = {
                .Id = SerializeString(reinterpret_cast<const char8_t*>(Config::GetProjectName())),
                .Name = SerializeString(reinterpret_cast<const char8_t*>(Config::GetProjectName())),
                .Version = {
                    .Humanized = SerializeString(reinterpret_cast<const char8_t*>(Config::GetVersionLong())),
                    .Numeric = Config::GetVersionNumeric()
                }
            },

            .ArchiveGetIdentity = &InterfaceWrapper::ArchiveGetIdentity,
            .ArchiveSetIdentity = &InterfaceWrapper::ArchiveSetIdentity,
            .ArchiveGetStreamCount = &InterfaceWrapper::ArchiveGetStreamCount,
            .ArchiveGetSectorSize = &InterfaceWrapper::ArchiveGetSectorSize,
            .ArchiveGetStreamIdxFromId = &InterfaceWrapper::ArchiveGetStreamIdxFromId,
            .ArchiveOpenStreamRead = &InterfaceWrapper::ArchiveOpenStreamRead,
            .ArchiveOpenStreamWrite = &InterfaceWrapper::ArchiveOpenStreamWrite,
            .ArchiveCloseStream = &InterfaceWrapper::ArchiveCloseStream,

            .ArchiveStreamGetIdentity = &InterfaceWrapper::ArchiveStreamGetIdentity,
            .ArchiveStreamSetIdentity = &InterfaceWrapper::ArchiveStreamSetIdentity,
            .ArchiveStreamGetElementSize = &InterfaceWrapper::ArchiveStreamGetElementSize,
            .ArchiveStreamSetElementSize = &InterfaceWrapper::ArchiveStreamSetElementSize,
            .ArchiveStreamReadContext = &InterfaceWrapper::ArchiveStreamReadContext,
            .ArchiveStreamWriteContext = &InterfaceWrapper::ArchiveStreamWriteContext,
            .ArchiveStreamGetCapacity = &InterfaceWrapper::ArchiveStreamGetCapacity,
            .ArchiveStreamGetSize = &InterfaceWrapper::ArchiveStreamGetSize,
            .ArchiveStreamReserveCapacity = &InterfaceWrapper::ArchiveStreamReserveCapacity,
            .ArchiveStreamShrinkToFit = &InterfaceWrapper::ArchiveStreamShrinkToFit,
            .ArchiveStreamResize = &InterfaceWrapper::ArchiveStreamResize,
            .ArchiveStreamReadBytes = &InterfaceWrapper::ArchiveStreamReadBytes,
            .ArchiveStreamWriteBytes = &InterfaceWrapper::ArchiveStreamWriteBytes,

            .UpdateGetState = &InterfaceWrapper::UpdateGetState,
            .UpdateOnStart = &InterfaceWrapper::UpdateOnStart,
            .UpdateOnProgress = &InterfaceWrapper::UpdateOnProgress,
            .UpdateOnPieceUpdate = &InterfaceWrapper::UpdateOnPieceUpdate,
            .UpdateOnFinalize = &InterfaceWrapper::UpdateOnFinalize,
            .UpdateOnComplete = &InterfaceWrapper::UpdateOnComplete
        };
        return Interface;
    }

    const SourceLibrary* SourceManager::GetSource(std::u8string_view Id) const
    {
        auto Itr = std::ranges::find(Sources, Id, [](const SourceLibrary& Source) {
            return DeserializeString(Source.GetIdentity().Id);
        });
        if (Itr != Sources.end())
        {
            return &*Itr;
        }
        return nullptr;
    }

    SourceArchive& GetArchive(Source::Archive Archive)
    {
        auto Ptr = reinterpret_cast<SourceArchive*>(Archive);
        if (Ptr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }
        return *Ptr;
    }

    SourceStream& GetStream(Source::Stream Stream)
    {
        auto Ptr = reinterpret_cast<SourceStream*>(Stream);
        if (Ptr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }
        return *Ptr;
    }

    SourceUpdate& GetUpdate(Source::Update Update)
    {
        auto Ptr = reinterpret_cast<SourceUpdate*>(Update);
        if (Ptr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }
        return *Ptr;
    }

    void SourceManager::InterfaceWrapper::ArchiveGetIdentity(Source::Archive ArchiveCtx, Source::AppIdentity* OutIdentity)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        *OutIdentity = Archive.GetIdentity();
    }

    void SourceManager::InterfaceWrapper::ArchiveSetIdentity(Source::Archive ArchiveCtx, const Source::AppIdentity* NewIdentity)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        Archive.SetIdentity(*NewIdentity);
    }

    uint32_t SourceManager::InterfaceWrapper::ArchiveGetStreamCount(Source::Archive ArchiveCtx)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        return Archive.GetStreamCount();
    }

    uint32_t SourceManager::InterfaceWrapper::ArchiveGetSectorSize(Source::Archive ArchiveCtx)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        return Archive.GetSectorSize();
    }

    uint32_t SourceManager::InterfaceWrapper::ArchiveGetStreamIdxFromId(Source::Archive ArchiveCtx, const Source::String* Id)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        if (Id == nullptr)
        {
            throw std::invalid_argument("Id is null");
        }

        return Archive.GetStreamIdxFromId(DeserializeString(*Id));
    }

    void SourceManager::InterfaceWrapper::ArchiveOpenStreamRead(Source::Archive ArchiveCtx, uint32_t StreamIdx, Source::Stream* OutStream)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        if (OutStream == nullptr)
        {
            throw std::invalid_argument("OutStream is null");
        }

        *OutStream = Archive.OpenStreamRead(StreamIdx);
    }

    void SourceManager::InterfaceWrapper::ArchiveOpenStreamWrite(Source::Archive ArchiveCtx, uint32_t StreamIdx, Source::Stream* OutStream)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        if (OutStream == nullptr)
        {
            throw std::invalid_argument("OutStream is null");
        }

        *OutStream = Archive.OpenStreamWrite(StreamIdx);
    }

    void SourceManager::InterfaceWrapper::ArchiveCloseStream(Source::Archive ArchiveCtx, Source::Stream StreamCtx)
    {
        auto& Archive = GetArchive(ArchiveCtx);

        auto& Stream = GetStream(StreamCtx);

        Archive.CloseStream(Stream);
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamGetIdentity(Source::Stream StreamCtx, Source::StreamIdentity* OutIdentity)
    {
        auto& Stream = GetStream(StreamCtx);

        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        *OutIdentity = Stream.GetIdentity();
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamSetIdentity(Source::Stream StreamCtx, const Source::StreamIdentity* NewIdentity)
    {
        auto& Stream = GetStream(StreamCtx);

        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        Stream.SetIdentity(*NewIdentity);
    }

    uint32_t SourceManager::InterfaceWrapper::ArchiveStreamGetElementSize(Source::Stream StreamCtx)
    {
        auto& Stream = GetStream(StreamCtx);

        return Stream.GetElementSize();
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamSetElementSize(Source::Stream StreamCtx, uint32_t NewElementSize)
    {
        auto& Stream = GetStream(StreamCtx);

        return Stream.SetElementSize(NewElementSize);
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamReadContext(Source::Stream StreamCtx, void* Dst, uint32_t Size)
    {
        auto& Stream = GetStream(StreamCtx);

        if (Dst == nullptr)
        {
            throw std::invalid_argument("Dst is null");
        }

        if (Size != 184)
        {
            throw std::invalid_argument("Stream contexts are only allowed to be exactly 184 bytes");
        }

        std::ranges::copy(Stream.GetContext(), reinterpret_cast<std::byte*>(Dst));
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamWriteContext(Source::Stream StreamCtx, const void* Src, uint32_t Size)
    {
        auto& Stream = GetStream(StreamCtx);

        if (Src == nullptr)
        {
            throw std::invalid_argument("Src is null");
        }

        if (Size != 184)
        {
            throw std::invalid_argument("Stream contexts are only allowed to be exactly 184 bytes");
        }

        Stream.SetContext(std::span<const std::byte, 184>(reinterpret_cast<const std::byte*>(Src), 184));
    }

    uint64_t SourceManager::InterfaceWrapper::ArchiveStreamGetCapacity(Source::Stream StreamCtx)
    {
        auto& Stream = GetStream(StreamCtx);

        return Stream.GetCapacity();
    }

    uint64_t SourceManager::InterfaceWrapper::ArchiveStreamGetSize(Source::Stream StreamCtx)
    {
        auto& Stream = GetStream(StreamCtx);

        return Stream.GetSize();
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamReserveCapacity(Source::Stream StreamCtx, uint64_t NewCapacity)
    {
        auto& Stream = GetStream(StreamCtx);

        Stream.ReserveCapacity(NewCapacity);
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamShrinkToFit(Source::Stream StreamCtx)
    {
        auto& Stream = GetStream(StreamCtx);

        Stream.ShrinkToFit();
    }

    void SourceManager::InterfaceWrapper::ArchiveStreamResize(Source::Stream StreamCtx, uint64_t NewSize)
    {
        auto& Stream = GetStream(StreamCtx);

        Stream.Resize(NewSize);
    }

    uint64_t SourceManager::InterfaceWrapper::ArchiveStreamReadBytes(Source::Stream StreamCtx, void* Dst, uint64_t Size, uint64_t Offset)
    {
        auto& Stream = GetStream(StreamCtx);

        if (Dst == nullptr)
        {
            throw std::invalid_argument("Dst is null");
        }

        return Stream.Read({ reinterpret_cast<std::byte*>(Dst), Size }, Offset);
    }

    uint64_t SourceManager::InterfaceWrapper::ArchiveStreamWriteBytes(Source::Stream StreamCtx, const void* Src, uint64_t Size, uint64_t Offset)
    {
        auto& Stream = GetStream(StreamCtx);

        if (Src == nullptr)
        {
            throw std::invalid_argument("Src is null");
        }

        return Stream.Write({ reinterpret_cast<const std::byte*>(Src), Size }, Offset);
    }
    
    Source::UpdateState SourceManager::InterfaceWrapper::UpdateGetState(Source::Update UpdateCtx)
    {
        auto& Update = GetUpdate(UpdateCtx);

        return Update.GetState();
    }

    void SourceManager::InterfaceWrapper::UpdateOnStart(Source::Update UpdateCtx, const Source::UpdateStartInfo* StartInfo)
    {
        auto& Update = GetUpdate(UpdateCtx);

        if (StartInfo == nullptr)
        {
            throw std::invalid_argument("StartInfo is null");
        }

        Update.OnStart(*StartInfo);
    }

    void SourceManager::InterfaceWrapper::UpdateOnProgress(Source::Update UpdateCtx, const Source::UpdateProgressInfo* ProgressInfo)
    {
        auto& Update = GetUpdate(UpdateCtx);

        if (ProgressInfo == nullptr)
        {
            throw std::invalid_argument("ProgressInfo is null");
        }

        Update.OnProgress(*ProgressInfo);
    }

    void SourceManager::InterfaceWrapper::UpdateOnPieceUpdate(Source::Update UpdateCtx, uint64_t Id, Source::UpdatePieceStatus NewStatus)
    {
        auto& Update = GetUpdate(UpdateCtx);

        Update.OnPieceUpdate(Id, NewStatus);
    }

    void SourceManager::InterfaceWrapper::UpdateOnFinalize(Source::Update UpdateCtx)
    {
        auto& Update = GetUpdate(UpdateCtx);

        Update.OnFinalize();
    }

    void SourceManager::InterfaceWrapper::UpdateOnComplete(Source::Update UpdateCtx)
    {
        auto& Update = GetUpdate(UpdateCtx);

        Update.OnComplete();
    }
}