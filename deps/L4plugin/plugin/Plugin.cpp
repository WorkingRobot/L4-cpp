#include "Plugin.h"

namespace L4::Plugin::Manager
{
    void Plugin::Initialize()
    {
        auto ClientInterface = GetRawInterface();
        InitializeProc(&ClientInterface, &Interface);
    }

    libL4::ClientInterface Plugin::GetRawInterface()
    {
        return {
            .InterfaceVersion = libL4::InterfaceVersion::Latest,
            .Identity = libL4::Marshal::To(ClientIdentity),
            .PluginHandle = libL4::Handle(this),
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
            }
        };
    }

    void Plugin::RawOps::Auth::OnUserUpdated(libL4::Handle Plugin)
    {
        auto PluginPtr = (Manager::Plugin*)Plugin;

        if (PluginPtr == nullptr)
        {
            throw std::invalid_argument("Plugin is null");
        }

        PluginPtr->Auth.OnUserUpdated();
    }

    void Plugin::RawOps::Archive::GetIdentity(libL4::Handle Archive, libL4::ArchiveIdentity* OutIdentity)
    {
    }

    void Plugin::RawOps::Archive::SetIdentity(libL4::Handle Archive, const libL4::ArchiveIdentity* NewIdentity)
    {
    }

    uint32_t Plugin::RawOps::Archive::GetStreamCount(libL4::Handle Archive)
    {
    }

    uint32_t Plugin::RawOps::Archive::GetSectorSize(libL4::Handle Archive)
    {
    }

    uint32_t Plugin::RawOps::Archive::GetStreamIdxFromId(libL4::Handle Archive, const libL4::String* Id)
    {
    }

    void Plugin::RawOps::Archive::OpenStreamRead(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream)
    {
    }

    void Plugin::RawOps::Archive::OpenStreamWrite(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream)
    {
    }

    void Plugin::RawOps::Archive::CloseStream(libL4::Handle Archive, libL4::Handle Stream)
    {
    }

    void Plugin::RawOps::Stream::GetIdentity(libL4::Handle Stream, libL4::StreamIdentity* OutIdentity)
    {
    }

    void Plugin::RawOps::Stream::SetIdentity(libL4::Handle Stream, const libL4::StreamIdentity* NewIdentity)
    {
    }

    uint32_t Plugin::RawOps::Stream::GetElementSize(libL4::Handle Stream)
    {
    }

    void Plugin::RawOps::Stream::SetElementSize(libL4::Handle Stream, uint32_t NewElementSize)
    {
    }

    void Plugin::RawOps::Stream::ReadContext(libL4::Handle Stream, void* Dst, uint32_t Size)
    {
    }

    void Plugin::RawOps::Stream::WriteContext(libL4::Handle Stream, const void* Src, uint32_t Size)
    {
    }

    uint64_t Plugin::RawOps::Stream::GetCapacity(libL4::Handle Stream)
    {
    }

    uint64_t Plugin::RawOps::Stream::GetSize(libL4::Handle Stream)
    {
    }

    void Plugin::RawOps::Stream::ReserveCapacity(libL4::Handle Stream, uint64_t NewCapacity)
    {
    }

    void Plugin::RawOps::Stream::ShrinkToFit(libL4::Handle Stream)
    {
    }

    void Plugin::RawOps::Stream::Resize(libL4::Handle Stream, uint64_t NewSize)
    {
    }

    uint64_t Plugin::RawOps::Stream::ReadBytes(libL4::Handle Stream, void* Dst, uint64_t Size, uint64_t Offset)
    {
    }

    uint64_t Plugin::RawOps::Stream::WriteBytes(libL4::Handle Stream, const void* Src, uint64_t Size, uint64_t Offset)
    {
    }

    libL4::UpdateState Plugin::RawOps::Update::GetState(libL4::Handle Update)
    {
    }

    void Plugin::RawOps::Update::OnStart(libL4::Handle Update, const libL4::UpdateStartInfo* StartInfo)
    {
    }

    void Plugin::RawOps::Update::OnProgress(libL4::Handle Update, const libL4::UpdateProgressInfo* ProgressInfo)
    {
    }

    void Plugin::RawOps::Update::OnPieceUpdate(libL4::Handle Update, uint64_t Id, libL4::UpdatePieceStatus NewStatus)
    {
    }

    void Plugin::RawOps::Update::OnFinalize(libL4::Handle Update)
    {
    }

    void Plugin::RawOps::Update::OnComplete(libL4::Handle Update)
    {
    }
}