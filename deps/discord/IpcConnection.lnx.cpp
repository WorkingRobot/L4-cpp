#include "IpcConnection.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <filesystem>

namespace L4::Discord
{
    int IpcConnection::GetProcessId()
    {
        return getpid();
    }

    IpcConnection::IpcConnection() :
        PipeHandle(-1),
        IsPipeOpen(false)
    {
    }

    static std::filesystem::path GetTempFilePath()
    {
        auto XDGRuntime = getenv("XDG_RUNTIME_DIR");
        if (XDGRuntime != nullptr)
        {
            return XDGRuntime;
        }
        return std::filesystem::temp_directory_path();
    }

    bool IpcConnection::Connect()
    {
        PipeHandle = socket(AF_UNIX, SOCK_STREAM, 0);
        if (PipeHandle == -1)
        {
            return false;
        }
        fcntl(PipeHandle, F_SETFL, O_NONBLOCK);
        auto PipePath = GetTempFilePath() / "discord-ipc-0";
        sockaddr_un SockAddr { .sun_family = AF_UNIX };
        strncpy(SockAddr.sun_path, PipePath.c_str(), sizeof(SockAddr.sun_path));
        char& PipePathNum = SockAddr.sun_path[PipePath.native().size() - 1];

        // Try digits 0-9 consecutively
        for (; PipePathNum < '9'; PipePathNum++)
        {
            if (connect(PipeHandle, reinterpret_cast<sockaddr*>(&SockAddr), sizeof(SockAddr)) == 0)
            {
                IsPipeOpen = true;
                return true;
            }
        };
        Disconnect();
        return false;
    }

    bool IpcConnection::Disconnect()
    {
        close(PipeHandle);
        PipeHandle = -1;
        IsPipeOpen = false;

        return true;
    }

    bool IpcConnection::WriteBytes(const void* Data, size_t Size)
    {
        if (Size == 0)
        {
            return true;
        }

        if (PipeHandle == -1)
        {
            return false;
        }

        if (Data == nullptr)
        {
            return false;
        }

        auto BytesWritten = send(PipeHandle, Data, Size, MSG_NOSIGNAL);
        if (BytesWritten == -1)
        {
            Disconnect();
        }
        return BytesWritten == Size;
    }

    bool IpcConnection::ReadBytes(void* Data, size_t Size)
    {
        if (Size == 0)
        {
            return true;
        }

        if (PipeHandle == -1)
        {
            return false;
        }

        if (Data == nullptr)
        {
            return false;
        }

        auto BytesRead = recv(PipeHandle, Data, Size, MSG_NOSIGNAL);

        return BytesRead == Size;
    }
}