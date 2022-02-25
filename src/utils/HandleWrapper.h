#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    class HandleWrapper
    {
    public:
        HandleWrapper(HANDLE&& Handle) : Handle(Handle)
        {

        }

        HandleWrapper(const HandleWrapper&) = delete;

        HandleWrapper(HandleWrapper&& Other) noexcept : Handle(std::exchange(Other.Handle, INVALID_HANDLE_VALUE))
        {

        }

        ~HandleWrapper()
        {
            CloseHandle(Handle);
        }

        operator HANDLE() const noexcept
        {
            return Handle;
        }

    private:
        HANDLE Handle;
    };
}