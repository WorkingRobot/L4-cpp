#pragma once

#include "SourceStructs.h"

#include <chrono>
#include <functional>

namespace L4::Riot
{
    class Update
    {
    public:
        Update(const Source::AppIdentity& OldIdentity, Source::AppIdentity& NewIdentity);

        void Start(Source::Archive Archive, std::chrono::milliseconds ProgressUpdateRate);

        void Pause();

        void Resume();

        struct CallbackList
        {
            std::function<Source::UpdateState()> GetState;
            std::function<void(const Source::UpdateStartInfo&)> OnStart;
            std::function<void(const Source::UpdateProgressInfo&)> OnProgress;
            std::function<void(uint64_t, Source::UpdatePieceStatus)> OnPieceUpdate;
            std::function<void()> OnFinalize;
            std::function<void()> OnComplete;
        };

        void SetCallbacks(CallbackList Callbacks);

    protected:
        Source::UpdateState GetState();

        void OnStart(const Source::UpdateStartInfo& StartInfo);

        void OnProgress(const Source::UpdateProgressInfo& ProgressInfo);

        void OnPieceUpdate(uint64_t Id, Source::UpdatePieceStatus NewStatus);

        void OnFinalize();

        void OnComplete();

    private:
        CallbackList Callbacks;
    };
}