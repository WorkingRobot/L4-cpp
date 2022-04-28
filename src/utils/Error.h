#pragma once

#include <ntdll.h>
#include <system_error>

namespace L4 {
    template <class T = std::system_error, class... ArgTs>
    static T CreateErrorWin32(DWORD Error, ArgTs&&... Args)
    {
        return { (int)Error, std::system_category(), std::forward<ArgTs>(Args)... };
    }

    template <class T = std::system_error, class... ArgTs>
    static T CreateErrorNtStatus(NTSTATUS Error, ArgTs&&... Args)
    {
        return CreateErrorWin32<T>(RtlNtStatusToDosError(Error), std::forward<ArgTs>(Args)...);
    }
}