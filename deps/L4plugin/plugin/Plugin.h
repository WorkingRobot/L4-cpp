#pragma once

#include "Auth.h"

#include <libL4/Interface.h>
#include <libL4/marshal/Marshal.h>

#include <filesystem>

namespace L4::Plugin::Manager
{
    class Plugin
    {
    public:
        Plugin(const std::filesystem::path& FilePath, const libL4::Marshal::ClientIdentity& ClientIdentity, const std::filesystem::path& ConfigBaseDirectory);

        ~Plugin();

        void Initialize();

    private:
        libL4::ClientInterface GetRawInterface();

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
                static uint32_t GetStreamIdxFromId(libL4::Handle Archive, const libL4::String* Id);
                static void OpenStreamRead(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream);
                static void OpenStreamWrite(libL4::Handle Archive, uint32_t StreamIdx, libL4::Handle* OutStream);
                static void CloseStream(libL4::Handle Archive, libL4::Handle Stream);
            };

            struct Stream
            {
                static void GetIdentity(libL4::Handle Stream, libL4::StreamIdentity* OutIdentity);
                static void SetIdentity(libL4::Handle Stream, const libL4::StreamIdentity* NewIdentity);
                static uint32_t GetElementSize(libL4::Handle Stream);
                static void SetElementSize(libL4::Handle Stream, uint32_t NewElementSize);
                static void ReadContext(libL4::Handle Stream, void* Dst, uint32_t Size);
                static void WriteContext(libL4::Handle Stream, const void* Src, uint32_t Size);
                static uint64_t GetCapacity(libL4::Handle Stream);
                static uint64_t GetSize(libL4::Handle Stream);
                static void ReserveCapacity(libL4::Handle Stream, uint64_t NewCapacity);
                static void ShrinkToFit(libL4::Handle Stream);
                static void Resize(libL4::Handle Stream, uint64_t NewSize);
                static uint64_t ReadBytes(libL4::Handle Stream, void* Dst, uint64_t Size, uint64_t Offset);
                static uint64_t WriteBytes(libL4::Handle Stream, const void* Src, uint64_t Size, uint64_t Offset);
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

        using GetInitializerProc = libL4::Initializer (*)();

        libL4::Marshal::ClientIdentity ClientIdentity;
        std::filesystem::path ConfigBaseDirectory;
        void* ModuleHandle;
        libL4::Initializer InitializeProc;
        libL4::PluginInterface Interface;

        Auth Auth;
    };
}