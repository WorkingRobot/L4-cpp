#include "ClientInterface.h"

#include "IArchive.h"
#include "IUpdate.h"
#include "Plugin.h"

namespace L4::Manager
{
    ClientInterface::ClientInterface(const libL4::Marshal::ClientIdentity& Identity, const std::filesystem::path& ConfigBaseDirectory)
    {
        RawInterface = {
            .InterfaceVersion = libL4::InterfaceVersion::Initial,
            .Identity = libL4::Marshal::To(Identity),
            .ConfigBaseDirectory = libL4::Marshal::To(ConfigBaseDirectory.u8string()),
            .Auth = {
                .OnUserUpdated = RawOps::Auth::OnUserUpdated,
            },
            .Archive = {
                .GetIdentity = RawOps::Archive::GetIdentity,
                .SetIdentity = RawOps::Archive::SetIdentity,
                .GetStreamCount = RawOps::Archive::GetStreamCount,
                .GetSectorSize = RawOps::Archive::GetSectorSize,
                .GetStreamIdxFromId = RawOps::Archive::GetStreamIdxFromId,
                .OpenStreamRead = RawOps::Archive::OpenStreamRead,
                .OpenStreamWrite = RawOps::Archive::OpenStreamWrite,
                .CloseStream = RawOps::Archive::CloseStream,
            },
            .Stream = {
                .GetIdentity = RawOps::Stream::GetIdentity,
                .SetIdentity = RawOps::Stream::SetIdentity,
                .GetElementSize = RawOps::Stream::GetElementSize,
                .SetElementSize = RawOps::Stream::SetElementSize,
                .ReadContext = RawOps::Stream::ReadContext,
                .WriteContext = RawOps::Stream::WriteContext,
                .GetCapacity = RawOps::Stream::GetCapacity,
                .GetSize = RawOps::Stream::GetSize,
                .ReserveCapacity = RawOps::Stream::ReserveCapacity,
                .ShrinkToFit = RawOps::Stream::ShrinkToFit,
                .Resize = RawOps::Stream::Resize,
                .ReadBytes = RawOps::Stream::ReadBytes,
                .WriteBytes = RawOps::Stream::WriteBytes,
            },
            .Update = {
                .GetState = RawOps::Update::GetState,
                .OnStart = RawOps::Update::OnStart,
                .OnProgress = RawOps::Update::OnProgress,
                .OnPieceUpdate = RawOps::Update::OnPieceUpdate,
                .OnFinalize = RawOps::Update::OnFinalize,
                .OnComplete = RawOps::Update::OnComplete,
            },
        };
    }

    const libL4::ClientInterface& ClientInterface::GetRawInterface() const noexcept
    {
        return RawInterface;
    }

    void ClientInterface::RawOps::Auth::OnUserUpdated(libL4::Handle Plugin)
    {
        auto PluginPtrPtr = (Manager::Plugin**)Plugin;
        if (PluginPtrPtr == nullptr)
        {
            throw std::invalid_argument("Plugin is null");
        }

        auto PluginPtr = *PluginPtrPtr;
        if (PluginPtr == nullptr)
        {
            throw std::invalid_argument("Plugin is invalid");
        }

        PluginPtr->OnAuthUserUpdated();
    }

    void ClientInterface::RawOps::Archive::GetIdentity(libL4::Handle Archive, libL4::ArchiveIdentity* OutIdentity)
    {
        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        auto Ret = ArchivePtr->GetIdentity();
        *OutIdentity = libL4::Marshal::To(Ret);
    }

    void ClientInterface::RawOps::Archive::SetIdentity(libL4::Handle Archive, const libL4::ArchiveIdentity* NewIdentity)
    {
        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        ArchivePtr->SetIdentity(libL4::Marshal::To(*NewIdentity));
    }

    uint32_t ClientInterface::RawOps::Archive::GetStreamCount(libL4::Handle Archive)
    {
        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        return ArchivePtr->GetStreamCount();
    }

    uint32_t ClientInterface::RawOps::Archive::GetSectorSize(libL4::Handle Archive)
    {
        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        return ArchivePtr->GetSectorSize();
    }

    uint32_t ClientInterface::RawOps::Archive::GetStreamIdxFromId(libL4::Handle Archive, const libL4::String* Id)
    {
        if (Id == nullptr)
        {
            throw std::invalid_argument("Id is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        return ArchivePtr->GetStreamIdxFromId(libL4::Marshal::To(*Id));
    }

    void ClientInterface::RawOps::Archive::OpenStreamRead(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream)
    {
        if (OutStream == nullptr)
        {
            throw std::invalid_argument("OutStream is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        *OutStream = ArchivePtr->OpenStreamInternal(StreamIdx, false);
    }

    void ClientInterface::RawOps::Archive::OpenStreamWrite(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream)
    {
        if (OutStream == nullptr)
        {
            throw std::invalid_argument("OutStream is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        *OutStream = ArchivePtr->OpenStreamInternal(StreamIdx, true);
    }

    void ClientInterface::RawOps::Archive::CloseStream(libL4::Handle Archive, libL4::Handle Stream)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        auto ArchivePtr = (IArchive*)Archive;
        if (ArchivePtr == nullptr)
        {
            throw std::invalid_argument("Archive is null");
        }

        if (!ArchivePtr->CloseStreamInternal(Stream))
        {
            throw std::invalid_argument("Stream does not exist");
        }
    }

    void ClientInterface::RawOps::Stream::GetIdentity(libL4::Handle Stream, libL4::StreamIdentity* OutIdentity)
    {
        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        auto Ret = StreamPtr->GetIdentity();
        *OutIdentity = libL4::Marshal::To(Ret);
    }

    void ClientInterface::RawOps::Stream::SetIdentity(libL4::Handle Stream, const libL4::StreamIdentity* NewIdentity)
    {
        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->SetIdentity(libL4::Marshal::To(*NewIdentity));
    }

    uint32_t ClientInterface::RawOps::Stream::GetElementSize(libL4::Handle Stream)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        return StreamPtr->GetElementSize();
    }

    void ClientInterface::RawOps::Stream::SetElementSize(libL4::Handle Stream, uint32_t NewElementSize)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->SetElementSize(NewElementSize);
    }

    void ClientInterface::RawOps::Stream::ReadContext(libL4::Handle Stream, void* Dst, uint32_t Size)
    {
        if (Dst == nullptr)
        {
            throw std::invalid_argument("Dst is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->ReadContext(std::span((std::byte*)Dst, Size));
    }

    void ClientInterface::RawOps::Stream::WriteContext(libL4::Handle Stream, const void* Src, uint32_t Size)
    {
        if (Src == nullptr)
        {
            throw std::invalid_argument("Src is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->WriteContext(std::span((std::byte*)Src, Size));
    }

    uint64_t ClientInterface::RawOps::Stream::GetCapacity(libL4::Handle Stream)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        return StreamPtr->GetCapacity();
    }

    uint64_t ClientInterface::RawOps::Stream::GetSize(libL4::Handle Stream)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        return StreamPtr->GetSize();
    }

    void ClientInterface::RawOps::Stream::ReserveCapacity(libL4::Handle Stream, uint64_t NewCapacity)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->ReserveCapacity(NewCapacity);
    }

    void ClientInterface::RawOps::Stream::ShrinkToFit(libL4::Handle Stream)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->ShrinkToFit();
    }

    void ClientInterface::RawOps::Stream::Resize(libL4::Handle Stream, uint64_t NewSize)
    {
        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        StreamPtr->Resize(NewSize);
    }

    uint64_t ClientInterface::RawOps::Stream::ReadBytes(libL4::Handle Stream, void* Dst, uint64_t Size, uint64_t Offset)
    {
        if (Dst == nullptr)
        {
            throw std::invalid_argument("Dst is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        return StreamPtr->ReadBytes(std::span((std::byte*)Dst, Size), Offset);
    }

    uint64_t ClientInterface::RawOps::Stream::WriteBytes(libL4::Handle Stream, const void* Src, uint64_t Size, uint64_t Offset)
    {
        if (Src == nullptr)
        {
            throw std::invalid_argument("Src is null");
        }

        auto StreamPtr = (IStream*)Stream;
        if (StreamPtr == nullptr)
        {
            throw std::invalid_argument("Stream is null");
        }

        return StreamPtr->WriteBytes(std::span((std::byte*)Src, Size), Offset);
    }

    libL4::UpdateState ClientInterface::RawOps::Update::GetState(libL4::Handle Update)
    {
        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        return UpdatePtr->GetState();
    }

    void ClientInterface::RawOps::Update::OnStart(libL4::Handle Update, const libL4::UpdateStartInfo* StartInfo)
    {
        if (StartInfo == nullptr)
        {
            throw std::invalid_argument("StartInfo is null");
        }

        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        UpdatePtr->OnStart(libL4::Marshal::To(*StartInfo));
    }

    void ClientInterface::RawOps::Update::OnProgress(libL4::Handle Update, const libL4::UpdateProgressInfo* ProgressInfo)
    {
        if (ProgressInfo == nullptr)
        {
            throw std::invalid_argument("ProgressInfo is null");
        }

        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        UpdatePtr->OnProgress(libL4::Marshal::To(*ProgressInfo));
    }

    void ClientInterface::RawOps::Update::OnPieceUpdate(libL4::Handle Update, uint64_t Id, libL4::UpdatePieceStatus NewStatus)
    {
        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        UpdatePtr->OnPieceUpdate(Id, NewStatus);
    }

    void ClientInterface::RawOps::Update::OnFinalize(libL4::Handle Update)
    {
        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        UpdatePtr->OnFinalize();
    }

    void ClientInterface::RawOps::Update::OnComplete(libL4::Handle Update)
    {
        auto UpdatePtr = (IUpdate*)Update;
        if (UpdatePtr == nullptr)
        {
            throw std::invalid_argument("Update is null");
        }

        UpdatePtr->OnComplete();
    }
}