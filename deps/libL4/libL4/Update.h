#pragma once

#include "Archive.h"

namespace libL4
{
    typedef void* Update;

    enum class UpdateState : uint8_t
    {
        Unknown,
        Starting,
        Running,
        Paused,
        Finalizing,
        Complete
    };

    enum class UpdatePieceStatus : uint8_t
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

    struct UpdateStartInfo
    {
        uint64_t TotalPieceCount;
        uint64_t TotalDownloadByteCount;
        uint64_t TotalReadByteCount;
        uint64_t TotalWriteByteCount;
    };

    struct UpdateProgressInfo
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

    struct UpdateOperations
    {
        UpdateState (*GetState)(Update Update);
        void (*OnStart)(Update Update, const UpdateStartInfo* StartInfo);
        void (*OnProgress)(Update Update, const UpdateProgressInfo* ProgressInfo);
        void (*OnPieceUpdate)(Update Update, uint64_t Id, UpdatePieceStatus NewStatus);
        void (*OnFinalize)(Update Update);
        void (*OnComplete)(Update Update);
    };

    struct UpdateCallbacks
    {
        // L4 passes a new Update object, the source grabs the new current version and attaches some context to keep track of it
        void (*Open)(Update Update, const AppIdentity* OldIdentity, AppIdentity* NewIdentity);

        // Stop the update if started, close the Update object, it won't be referenced again (the source should dispose of its internal context data and its archive if it's not nullptr)
        void (*Close)(Update Update);

        // Begin an actual update sequence, archive object is valid, source can attach its context to it
        void (*Start)(Update Update, Archive Archive, uint32_t ProgressUpdateRateMs);

        // Pause the update sequence. Can call UpdateResume to resume the update
        void (*Pause)(Update Update);

        // Resume the update sequence. Called after UpdatePause
        void (*Resume)(Update Update);
    };
}