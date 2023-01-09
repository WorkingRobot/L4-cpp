#pragma once

#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Manager
{
    class ClientInterface
    {
    public:
        ClientInterface(const libL4::Marshal::ClientIdentity& Identity, const std::filesystem::path& ConfigBaseDirectory);

        const libL4::ClientInterface& GetRawInterface() const noexcept;

    private:
        struct RawOps
        {
            struct Auth
            {
                static void OnUserUpdated(libL4::Handle Plugin);
            };

            struct Archive
            {
                static void GetIdentity(libL4::Handle Archive, libL4::ArchiveIdentity* OutIdentity);
                static void SetIdentity(libL4::Handle Archive, const libL4::ArchiveIdentity* NewIdentity);
                static uint32_t GetStreamCount(libL4::Handle Archive);
                static uint32_t GetSectorSize(libL4::Handle Archive);
                static void OpenStream(libL4::Handle Archive, const libL4::String* Id, libL4::Handle* OutStream);
                static void CloseStream(libL4::Handle Archive, libL4::Handle Stream);
            };

            struct Stream
            {
                static void GetIdentity(libL4::Handle Stream, libL4::StreamIdentity* OutIdentity);
                static void SetIdentity(libL4::Handle Stream, const libL4::StreamIdentity* NewIdentity);
                static void ReadContext(libL4::Handle Stream, void* Dst, uint32_t Size);
                static void WriteContext(libL4::Handle Stream, const void* Src, uint32_t Size);
                static uint64_t GetCapacity(libL4::Handle Stream);
                static uint64_t GetSize(libL4::Handle Stream);
                static void ReserveCapacity(libL4::Handle Stream, uint64_t NewCapacity);
                static void ShrinkToFit(libL4::Handle Stream);
                static void Resize(libL4::Handle Stream, uint64_t NewSize);
                static void ReadBytes(libL4::Handle Stream, void* Dst, uint64_t Size, uint64_t Offset);
                static void WriteBytes(libL4::Handle Stream, const void* Src, uint64_t Size, uint64_t Offset);
            };

            struct Update
            {
                static libL4::UpdateState GetState(libL4::Handle Update);
                static void OnStart(libL4::Handle Update, const libL4::UpdateStartInfo* StartInfo);
                static void OnProgress(libL4::Handle Update, const libL4::UpdateProgressInfo* ProgressInfo);
                static void OnPieceUpdate(libL4::Handle Update, uint64_t Id, libL4::UpdatePieceStatus NewStatus);
                static void OnFinalize(libL4::Handle Update);
                static void OnComplete(libL4::Handle Update);
            };
        };

        libL4::ClientInterface RawInterface;
    };
}