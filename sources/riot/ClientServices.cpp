#include "ClientServices.h"

#include "formatters/Path.h"
#include "KnownFolders.h"
#include "streams/FileStream.h"
#include "utils/HandleWrapper.h"
#include "web/Json.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4::Riot
{
    ClientServices::ClientServices()
    {
        auto LockData = GetLockFileData();
        if (LockData.has_value())
        {
            this->LockData = LockData.value();
            return;
        }

        StartProcess();
        WaitForLockFile();
        LockData = GetLockFileData();
        if (!LockData.has_value())
        {
            throw std::system_error(ERROR_FILE_NOT_FOUND, std::system_category(), "Could not get lock data after starting process");
        }
        this->LockData = LockData.value();
    }

    bool ClientServices::IsRunning()
    {
        return GetLockFileData().transform(IsLockFileDataValid).value_or(false);
    }

    std::optional<ClientServices::LockFileData> ClientServices::GetLockFileData()
    {
        auto LockFilePath = GetLockFilePath();
        if (LockFilePath.empty())
        {
            return std::nullopt;
        }

        if (std::error_code Code; !std::filesystem::is_regular_file(LockFilePath, Code))
        {
            return std::nullopt;
        }

        std::string DataString;
        try
        {
            DataString = DumpStreamData<FileStream>(LockFilePath, FileStream::OpenMode::Read, FileStream::CreateMode::OpenOnly, FileStream::ShareMode::DeleteReadWrite);
        }
        catch (const std::system_error& e)
        {
            return std::nullopt;
        }

        LockFileData LockFileData {};

        std::string_view DataView = DataString;
        for (int Idx = 0; Idx < 5; ++Idx)
        {
            if (DataView.empty())
            {
                return std::nullopt;
            }

            auto IdxView = DataView;
            auto ColonPos = DataView.find(':');
            if (ColonPos != std::string_view::npos)
            {
                IdxView = IdxView.substr(0, ColonPos);
                DataView.remove_prefix(ColonPos + 1);
            }

            if (IdxView.empty())
            {
                return std::nullopt;
            }

            switch (Idx)
            {
            case 0:
                LockFileData.Name = IdxView;
                break;
            case 1:
                if (std::from_chars(IdxView.data(), IdxView.data() + IdxView.size(), LockFileData.Pid).ptr != IdxView.data() + IdxView.size())
                {
                    return std::nullopt;
                }
                break;
            case 2:
                if (std::from_chars(IdxView.data(), IdxView.data() + IdxView.size(), LockFileData.Port).ptr != IdxView.data() + IdxView.size())
                {
                    return std::nullopt;
                }
                break;
            case 3:
                LockFileData.PrivateToken = IdxView;
                break;
            case 4:
                LockFileData.Scheme = IdxView;
                break;
            }
        }

        return LockFileData;
    }

    std::filesystem::path ClientServices::GetLockFilePath()
    {
        auto LockFilePath = GetKnownFolderPath(FOLDERID_LocalAppData);
        if (LockFilePath.empty())
        {
            return "";
        }

        LockFilePath /= "Riot Games";
        LockFilePath /= "Riot Client";
        LockFilePath /= "Config";
        LockFilePath /= "lockfile";

        return LockFilePath;
    }

    void WaitForFileCreation(const std::filesystem::path& Path)
    {
        if (std::error_code Code; std::filesystem::exists(Path, Code))
        {
            return;
        }

        auto DirectoryPath = Path.parent_path();
        HandleWrapper DirectoryHandle = CreateFileW(DirectoryPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (DirectoryHandle == INVALID_HANDLE_VALUE)
        {
            throw std::system_error(GetLastError(), std::system_category(), "Could not open handle to directory");
        }

        std::byte Buffer[1 << 12] {};
        while (true)
        {
            DWORD BytesReturned = 0;
            if (ReadDirectoryChangesW(DirectoryHandle, Buffer, sizeof(Buffer), FALSE, 0xF, &BytesReturned, NULL, NULL) == FALSE)
            {
                throw std::system_error(GetLastError(), std::system_category(), "Could not read directory changes");
            }

            if (BytesReturned == 0)
            {
                throw std::system_error(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "Buffer is too small");
            }

            auto* InformationPtr = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(Buffer);
            while (true)
            {
                if (InformationPtr->Action == FILE_NOTIFY_CHANGE_FILE_NAME)
                {
                    std::wstring_view FileName(InformationPtr->FileName, InformationPtr->FileNameLength / sizeof(WCHAR));
                    if (DirectoryPath / FileName == Path)
                    {
                        return;
                    }
                }

                if (InformationPtr->NextEntryOffset)
                {
                    InformationPtr = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<std::byte*>(InformationPtr) + InformationPtr->NextEntryOffset);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void ClientServices::WaitForLockFile()
    {
        WaitForFileCreation(GetLockFilePath());
    }

    std::filesystem::path ClientServices::GetProcessPath()
    {
        auto InstallsPath = GetKnownFolderPath(FOLDERID_ProgramData);
        if (InstallsPath.empty())
        {
            return "";
        }

        InstallsPath /= "Riot Games";
        InstallsPath /= "RiotClientInstalls.json";

        if (std::error_code Code; !std::filesystem::is_regular_file(InstallsPath, Code))
        {
            return "";
        }

        std::u8string InstallsDataString;
        try
        {
            InstallsDataString = DumpStreamData<FileStream, std::u8string>(InstallsPath, FileStream::OpenMode::Read, FileStream::CreateMode::OpenOnly, FileStream::ShareMode::DeleteReadWrite);
        }
        catch (const std::system_error& e)
        {
            return "";
        }

        auto InstallsData = Web::Json::Parse<>(InstallsDataString);
        if (InstallsData.HasParseError())
        {
            return "";
        }

        auto RcDefaultMember = InstallsData.FindMember(u8"rc_default");
        if (RcDefaultMember == InstallsData.MemberEnd())
        {
            return "";
        }

        if (!RcDefaultMember->value.IsString())
        {
            return "";
        }

        std::filesystem::path RcDefaultPath(std::u8string_view(RcDefaultMember->value.GetString(), RcDefaultMember->value.GetStringLength()));
        if (std::error_code Code; !std::filesystem::is_regular_file(RcDefaultPath, Code))
        {
            return "";
        }

        return RcDefaultPath;
    }

    void ClientServices::StartProcess()
    {
        auto Path = GetProcessPath();
        if (Path.empty())
        {
            throw std::system_error(ERROR_FILE_NOT_FOUND, std::system_category(), "Could not get process path");
        }

        STARTUPINFOW StartupInfo {
            .cb = sizeof(STARTUPINFOW)
        };
        PROCESS_INFORMATION ProcessInfo {};

        CreateProcessW(NULL, std::format(L"\"{:s}\" --insecure --headless", Path).data(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo);
    }

    bool IsPidRunning(DWORD Pid)
    {
        HandleWrapper ProcessHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Pid);

        if (ProcessHandle == NULL) {
            if (GetLastError() == ERROR_ACCESS_DENIED)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        DWORD ExitCode;
        if (GetExitCodeProcess(ProcessHandle, &ExitCode))
        {
            return true;
        }
    }

    bool ClientServices::IsLockFileDataValid(const LockFileData& LockData)
    {
        if (LockData.Name != "Riot Client")
        {
            return false;
        }

        if (LockData.Scheme != "https")
        {
            return false;
        }

        
    }
}