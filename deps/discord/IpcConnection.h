#pragma once

#include <string>

namespace L4::Discord
{
    class IpcConnection
    {
#if defined(CONFIG_VERSION_PLATFORM_lnx)
        using HandleT = int;
#elif defined(CONFIG_VERSION_PLATFORM_win)
        using HandleT = void*;
#endif

    public:
        IpcConnection();

        bool Connect();

        bool Disconnect();

        bool WriteBytes(const void* Data, size_t Size);

        bool ReadBytes(void* Data, size_t Size);

        bool IsOpen() const noexcept
        {
            return IsPipeOpen;
        }

        static int GetProcessId();

    private:
        HandleT PipeHandle;
        bool IsPipeOpen;
    };
}