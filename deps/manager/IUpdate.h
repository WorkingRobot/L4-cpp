#pragma once

#include <libL4/marshal/Marshal.h>

namespace L4::Manager
{
    class IUpdate
    {
    public:
        IUpdate() = default;

        virtual ~IUpdate() = default;

        virtual libL4::UpdateState GetState() const = 0;

        virtual void OnStart(const libL4::Marshal::UpdateStartInfo& StartInfo) = 0;

        virtual void OnProgress(const libL4::Marshal::UpdateProgressInfo& ProgressInfo) = 0;

        virtual void OnPieceUpdate(uint64_t Id, libL4::UpdatePieceStatus NewStatus) = 0;

        virtual void OnFinalize() = 0;

        virtual void OnComplete() = 0;
    };
}