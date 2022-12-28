#pragma once

#include "Archive.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <chrono>

namespace L4::Plugin::Wrapper
{
    class IUpdateSession
    {
    public:
        IUpdateSession(libL4::Handle ClientHandle);

        virtual ~IUpdateSession() = default;

        libL4::UpdateState GetState() const;

        void OnStart(const libL4::Marshal::UpdateStartInfo& Info);

        void OnProgress(const libL4::Marshal::UpdateProgressInfo& Info);

        void OnPieceUpdate(uint64_t Id, libL4::UpdatePieceStatus Status);

        void OnFinalize();

        void OnComplete();

        virtual void Start(Archive Archive, std::chrono::milliseconds ProgressUpdateRate) = 0;

        virtual void Pause() = 0;

        virtual void Resume() = 0;

    private:
        libL4::Handle ClientHandle;
    };
}