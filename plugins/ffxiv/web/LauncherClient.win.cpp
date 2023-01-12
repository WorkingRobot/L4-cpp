#include "LauncherClient.h"

#include <utils/Error.win.h>

#define WIN32_LEAN_AND_MEAN
#include <lmcons.h>
#include <Windows.h>

namespace L4::Plugin::FFXIV
{
    static std::u16string GetHostname()
    {
        std::u16string Name(MAX_COMPUTERNAME_LENGTH, '\0');
        DWORD NameSize = MAX_COMPUTERNAME_LENGTH + 1;
        if (!GetComputerNameW((wchar_t*)Name.data(), &NameSize))
        {
            throw CreateErrorWin32(GetLastError());
        }
        Name.resize(NameSize);
        return Name;
    }

    static std::u16string GetUsername()
    {
        std::u16string Name(UNLEN, '\0');
        DWORD NameSize = UNLEN + 1;
        if (!GetUserNameW((wchar_t*)Name.data(), &NameSize))
        {
            throw CreateErrorWin32(GetLastError());
        }
        Name.resize(NameSize - 1);
        return Name;
    }

    static std::u16string GetOSVersion()
    {
        OSVERSIONINFOEXA Version {
            .dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA)
        };
        if (!GetVersionExA((OSVERSIONINFOA*)&Version))
        {
            throw CreateErrorWin32(GetLastError());
        }
        // https://referencesource.microsoft.com/#mscorlib/system/environment.cs,1145
        // https://referencesource.microsoft.com/#mscorlib/system/operatingsystem.cs,129
        return fmt::format(u"Microsoft Windows NT {:d}.{:d}.{:d}.{:d}", Version.dwMajorVersion, Version.dwMinorVersion, Version.dwBuildNumber, (Version.wServicePackMajor << 16) | Version.wServicePackMinor);
    }

    static auto GetProcessorCount() noexcept
    {
        return std::thread::hardware_concurrency();
    }

    std::u16string GetComputerIdHashString()
    {
        return fmt::format(u"{:s}{:s}{:s}{:d}", GetHostname(), GetUsername(), GetOSVersion(), GetProcessorCount());
    }
}