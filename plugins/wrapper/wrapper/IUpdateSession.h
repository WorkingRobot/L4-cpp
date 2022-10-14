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

        virtual void Start(const Archive& Archive, std::chrono::milliseconds ProgressUpdateRate) = 0;

        virtual void Pause() = 0;

        virtual void Resume() = 0;
    };
}