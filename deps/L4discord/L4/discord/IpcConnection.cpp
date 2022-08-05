#include "IpcConnection.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4::Discord
{
    IpcConnection::IpcConnection() :
        PipeHandle(INVALID_HANDLE_VALUE),
        IsPipeOpen(false)
    {
    }

    bool IpcConnection::Connect()
    {
        char* PipeName = "\\\\?\\pipe\\discord-ipc-0";
        do
        {
            PipeHandle = CreateFile(PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

            if (PipeHandle != INVALID_HANDLE_VALUE)
            {
                IsPipeOpen = true;
                return true;
            }

            switch (GetLastError())
            {
            case ERROR_FILE_NOT_FOUND:
                // Try digits 0-9 consecutively
                if (PipeName[21] < '9')
                {
                    PipeName[21]++;
                    continue;
                }
                break;
            case ERROR_PIPE_BUSY:
                if (WaitNamedPipe(PipeName, 10000))
                {
                    continue;
                }
                break;
            }
            return false;
        } while (true);
        __assume(false);
    }

    bool IpcConnection::Disconnect()
    {
        CloseHandle(PipeHandle);
        PipeHandle = INVALID_HANDLE_VALUE;
        IsPipeOpen = false;

        return true;
    }

    bool IpcConnection::WriteBytes(const void* Data, size_t Size)
    {
        if (Size == 0)
        {
            return true;
        }

        if (PipeHandle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (Data == nullptr)
        {
            return false;
        }

        DWORD BytesWritten = 0;
        auto WriteSuccessful = WriteFile(PipeHandle, Data, Size, &BytesWritten, NULL);
        return WriteSuccessful && BytesWritten == Size;
    }

    bool IpcConnection::ReadBytes(void* Data, size_t Size)
    {
        if (Size == 0)
        {
            return true;
        }

        if (PipeHandle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (Data == nullptr)
        {
            return false;
        }

        DWORD BytesAvailable = 0;
        if (!PeekNamedPipe(PipeHandle, NULL, 0, NULL, &BytesAvailable, NULL))
        {
            return false;
        }
        if (BytesAvailable >= Size)
        {
            DWORD BytesRead = 0;
            auto ReadSuccessful = ReadFile(PipeHandle, Data, Size, &BytesRead, NULL);
            return ReadSuccessful && BytesRead == Size;
        }
        return false;
    }
}