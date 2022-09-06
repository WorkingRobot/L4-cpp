#include "LogImpl.h"

#include "Debug.h"

#include <fcntl.h>
#include <unistd.h>

namespace L4
{
    struct LoggerConsole
    {
        // Even if we try detecting the terminal with $TERM (which can lie), we can't really influence or force the terminal to *use* color or anything
        LoggerConsole() = default;

        void Log(LogLevel Level, const std::string& Message)
        {
            auto EscColorCode = LogLevelToEscColorCode(Level);

            write(STDOUT_FILENO, EscColorCode.data(), EscColorCode.size());
            write(STDOUT_FILENO, Message.c_str(), Message.size());
            write(STDOUT_FILENO, ResetEscColorCode.data(), ResetEscColorCode.size());
        }
    };

    struct LoggerFile
    {
        LoggerFile()
        {
            auto FilePath = GetLogFilePath();
            FileHandle = open(FilePath.c_str(), O_CREAT | O_WRONLY | O_APPEND);
            Ensure<LogLevel::Error>(FileHandle != -1, "Could not open {} for logging", [&] { return FMT::make_format_args(FilePath); });
        }

        ~LoggerFile() noexcept
        {
            close(FileHandle);
        }

        void Log(LogLevel Level, const std::string& Message)
        {
            if (FileHandle != -1)
            {
                write(FileHandle, Message.c_str(), Message.size());
            }
        }

    private:
        int FileHandle = -1;
    };

    namespace Debug
    {
        std::filesystem::path GetMinidumpDirectory()
        {
            return GetDumpFilePath().parent_path();
        }

        void HandleMinidump(const char* ExistingPath, bool Succeeded)
        {
            auto DumpPath = GetDumpFilePath();
            std::error_code Error;
            std::filesystem::rename(ExistingPath, DumpPath, Error);

            LogMiniDump(Error ? ExistingPath : DumpPath, Succeeded);
        }
    };
}

#include "LogImpl2.h"
