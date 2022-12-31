#pragma once

#include <libL4/marshal/Marshal.h>

namespace L4::Manager
{
    class UpdateSession
    {
    public:
        UpdateSession(libL4::UpdateCallbacks Callbacks, const libL4::Marshal::AppIdentity& Identity);

        ~UpdateSession();

        void Start();

        void Pause();

        void Resume();

    private:
        libL4::UpdateCallbacks Callbacks;
    };
}