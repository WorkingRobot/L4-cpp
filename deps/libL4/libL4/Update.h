#pragma once

#include "Archive.h"

namespace libL4
{
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
        UpdateStartInfo StartInfo;

        uint64_t PieceCount;

        uint64_t DownloadByteCount;
        uint64_t DownloadByteRate;

        uint64_t ReadByteCount;
        uint64_t ReadByteRate;

        uint64_t WriteByteCount;
        uint64_t WriteByteRate;
    };

    struct UpdateOperations
    {
        UpdateState (*GetState)(Handle Update);
        void (*OnStart)(Handle Update, const UpdateStartInfo* StartInfo);
        void (*OnProgress)(Handle Update, const UpdateProgressInfo* ProgressInfo);
        void (*OnPieceUpdate)(Handle Update, uint64_t Id, UpdatePieceStatus NewStatus);
        void (*OnFinalize)(Handle Update);
        void (*OnComplete)(Handle Update);
    };

    struct UpdateCallbacks
    {
        // L4 passes a new Update object, the source grabs the new current version and attaches some context to keep track of it
        void (*Open)(Handle Update, const AppIdentity* OldIdentity, AppIdentity* NewIdentity);

        // Stop the update if started, close the Update object, it won't be referenced again (the source should dispose of its internal context data and its archive if it's not nullptr)
        void (*Close)(Handle Update);

        // Begin an actual update sequence, archive object is valid, source can attach its context to it
        void (*Start)(Handle Update, Handle Archive, uint32_t ProgressUpdateRateMs);

        // Pause the update sequence. Can call UpdateResume to resume the update
        void (*Pause)(Handle Update);

        // Resume the update sequence. Called after UpdatePause
        void (*Resume)(Handle Update);
    };
}