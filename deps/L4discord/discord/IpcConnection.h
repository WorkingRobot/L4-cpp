#pragma once

#include <string>

namespace L4::Discord
{
    class IpcConnection
    {
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

    private:
        void* PipeHandle;
        bool IsPipeOpen;
    };
}