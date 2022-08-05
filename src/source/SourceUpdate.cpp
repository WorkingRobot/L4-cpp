#include "SourceUpdate.h"

#include "SourceLibrary.h"

namespace L4
{
    SourceUpdate::SourceUpdate(const SourceLibrary& Library, const Source::AppIdentity& OldIdentity) :
        Library(Library),
        OldIdentity(OldIdentity),
        NewIdentity(),
        UpdateState(Source::UpdateState::Unknown),
        IsUpdating(false),
        IsPaused(false)
    {
        Library.Interface.UpdateOpen(this, &this->OldIdentity, &NewIdentity);
    }

    SourceUpdate::~SourceUpdate()
    {
        Library.Interface.UpdateClose(this);
    }

    void SourceUpdate::Start(ArchiveWritable&& Archive, std::chrono::milliseconds UpdateRate)
    {
        if (IsUpdating)
        {
            throw std::invalid_argument("Update is already started");
        }

        auto& ArchiveRef = this->Archive.emplace(std::move(Archive));

        IsUpdating = true;
        UpdateState = Source::UpdateState::Starting;
        Library.Interface.UpdateStart(this, &ArchiveRef, UpdateRate.count());
    }

    void SourceUpdate::Pause()
    {
        if (!IsUpdating)
        {
            throw std::invalid_argument("Update hasn't started yet");
        }

        if (IsPaused)
        {
            throw std::invalid_argument("Update is already paused");
        }

        Library.Interface.UpdatePause(this);
        IsPaused = true;
        UpdateState = Source::UpdateState::Paused;
    }

    void SourceUpdate::Resume()
    {
        if (!IsUpdating)
        {
            throw std::invalid_argument("Update hasn't started yet");
        }

        if (!IsPaused)
        {
            throw std::invalid_argument("Update is already paused");
        }

        Library.Interface.UpdateResume(this);
        IsPaused = false;
        UpdateState = Source::UpdateState::Running;
    }

    Source::UpdateState SourceUpdate::GetState() const
    {
        return UpdateState;
    }

    void SourceUpdate::OnStart(const Source::UpdateStartInfo& StartInfo)
    {
        Callbacks.OnStart(StartInfo);

        UpdateState = Source::UpdateState::Running;
    }

    void SourceUpdate::OnProgress(const Source::UpdateProgressInfo& ProgressInfo)
    {
        Callbacks.OnProgress(ProgressInfo);
    }

    void SourceUpdate::OnPieceUpdate(uint64_t Id, Source::UpdatePieceStatus NewStatus)
    {
        Callbacks.OnPieceUpdate(Id, NewStatus);
    }

    void SourceUpdate::OnFinalize()
    {
        Callbacks.OnFinalize();

        UpdateState = Source::UpdateState::Finalizing;
    }

    void SourceUpdate::OnComplete()
    {
        Callbacks.OnComplete();

        UpdateState = Source::UpdateState::Complete;
    }

    void SourceUpdate::SetCallbacks(CallbackList Callbacks)
    {
        this->Callbacks = Callbacks;
    }
}