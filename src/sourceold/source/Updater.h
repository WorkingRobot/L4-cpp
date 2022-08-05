#pragma once

#include "Base.h"
#include "Archive.h"

#include <chrono>

namespace L4::Interface
{
    class UpstreamApp;

    enum class UpdaterState : uint8_t
    {
        Unknown,
        Initializing,
        Starting,
        Running,
        Finalizing,
        Done
    };
    
    enum class UpdaterPieceStatus : uint8_t
    {
        Unknown,
        Scheduled,
        Initializing,
        Transferring,
        Downloading,
        WritingMetadata,
        WritingData,
        Completed
    };

    struct UpdaterStartInfo
    {
        uint64_t TotalPieceCount;
        uint64_t TotalDownloadByteCount;
        uint64_t TotalReadByteCount;
        uint64_t TotalWriteByteCount;
    };

    struct UpdaterProgressUpdate
    {
        uint64_t TotalPieceCount;
        uint64_t CurrentPieceCount;

        uint64_t TotalDownloadByteCount;
        uint64_t CurrentDownloadByteCount;
        uint64_t DownloadByteRate;

        uint64_t TotalReadByteCount;
        uint64_t CurrentReadByteCount;
        uint64_t ReadByteRate;

        uint64_t TotalWriteByteCount;
        uint64_t CurrentWriteByteCount;
        uint64_t WriteByteRate;
    };

    class L4_CLASS_API UpdaterFI : public Interface
    {
    public:
        virtual Ptr<IArchiveWritable> GetArchive() const = 0;

        virtual Ptr<const UpstreamApp> GetUpstreamApp() const = 0;

        virtual std::chrono::milliseconds GetProgressUpdateRate() const = 0;

        virtual void OnStarted(const UpdaterStartInfo& Info) const = 0;

        virtual void OnProgressUpdate(const UpdaterProgressUpdate& Update) const = 0;

        virtual void OnPieceUpdate(uint64_t Id, UpdaterPieceStatus Status) const = 0;

        virtual void OnFinalize() const = 0;

        virtual void OnDone() const = 0;
    };

    class L4_CLASS_API IUpdater : public Object<UpdaterFI>
    {
    public:
        virtual void Start() const = 0;

        virtual void Pause() const = 0;

        virtual void Stop() const = 0;
    };
}