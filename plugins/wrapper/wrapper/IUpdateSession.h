#pragma once

#include "Archive.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    class IUpdateSession
    {
    public:
        IUpdateSession(Handle ClientHandle);

        virtual ~IUpdateSession() = default;

        Wrapper::UpdateState GetState() const;

        void OnStart(const Wrapper::UpdateStartInfo& Info);

        void OnProgress(const Wrapper::UpdateProgressInfo& Info);

        void OnPieceUpdate(uint64_t Id, Wrapper::UpdatePieceStatus Status);

        void OnFinalize();

        void OnComplete();

        virtual void Start(const Archive& Archive, std::chrono::milliseconds ProgressUpdateRate) = 0;

        virtual void Pause() = 0;

        virtual void Resume() = 0;

    private:
        Handle ClientHandle;
    };
}