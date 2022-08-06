#include "Update.h"

namespace L4::Riot
{
    Update::Update(const Source::AppIdentity& OldIdentity, Source::AppIdentity& NewIdentity)
    {
    }

    void Update::Start(Source::Archive Archive, std::chrono::milliseconds ProgressUpdateRate)
    {
    }

    void Update::Pause()
    {
    }

    void Update::Resume()
    {
    }

    void Update::SetCallbacks(CallbackList Callbacks)
    {
        this->Callbacks = Callbacks;
    }

    Source::UpdateState Update::GetState()
    {
        return Callbacks.GetState();
    }

    void Update::OnStart(const Source::UpdateStartInfo& StartInfo)
    {
        Callbacks.OnStart(StartInfo);
    }

    void Update::OnProgress(const Source::UpdateProgressInfo& ProgressInfo)
    {
        Callbacks.OnProgress(ProgressInfo);
    }

    void Update::OnPieceUpdate(uint64_t Id, Source::UpdatePieceStatus NewStatus)
    {
        Callbacks.OnPieceUpdate(Id, NewStatus);
    }

    void Update::OnFinalize()
    {
        Callbacks.OnFinalize();
    }

    void Update::OnComplete()
    {
        Callbacks.OnComplete();
    }
}