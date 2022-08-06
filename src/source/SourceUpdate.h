#pragma once

#include "SourceArchive.h"

#include <functional>

namespace L4
{
    class SourceLibrary;

    class SourceUpdate
    {
    public:
        SourceUpdate(const SourceLibrary& Library, const Source::AppIdentity& OldIdentity);

        ~SourceUpdate();

        void Start(ArchiveWritable&& Archive, std::chrono::milliseconds ProgressUpdateRate);

        void Pause();

        void Resume();

        Source::UpdateState GetState() const;

        void OnStart(const Source::UpdateStartInfo& StartInfo);

        void OnProgress(const Source::UpdateProgressInfo& ProgressInfo);

        void OnPieceUpdate(uint64_t Id, Source::UpdatePieceStatus NewStatus);

        void OnFinalize();

        void OnComplete();

        struct CallbackList
        {
            std::function<void(const Source::UpdateStartInfo&)> OnStart;
            std::function<void(const Source::UpdateProgressInfo&)> OnProgress;
            std::function<void(uint64_t, Source::UpdatePieceStatus)> OnPieceUpdate;
            std::function<void()> OnFinalize;
            std::function<void()> OnComplete;
        };

        void SetCallbacks(CallbackList Callbacks);

    private:
        const SourceLibrary& Library;

        Source::AppIdentity OldIdentity;
        Source::AppIdentity NewIdentity;

        Source::UpdateState UpdateState;
        bool IsUpdating;
        bool IsPaused;

        std::optional<SourceArchive> Archive;

        CallbackList Callbacks;
    };
}