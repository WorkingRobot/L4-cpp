#include "Log.h"

#include "Config.h"
#include "formatters/Path.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    static constexpr bool UseConsole = true;
    static constexpr bool CanCreateUIDialogs = true;

    struct LoggerConsole
    {
        void Setup()
        {
            if constexpr (!UseConsole)
            {
                return;
            }

            if (AllocConsole())
            {
                freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
                freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
            }

            StdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (StdOutHandle == INVALID_HANDLE_VALUE)
            {
                return;
            }

            DWORD CurrentMode = 0;
            if (!GetConsoleMode(StdOutHandle, &CurrentMode))
            {
                return;
            }

            ColorsEnabled = SetConsoleMode(StdOutHandle, CurrentMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }

        void Log(LogLevel Level, const std::string& Message)
        {
            if (StdOutHandle == INVALID_HANDLE_VALUE)
            {
                return;
            }

            if (ColorsEnabled)
            {
                auto EscColorCode = LogLevelToEscColorCode(Level);

                WriteConsole(StdOutHandle, EscColorCode.data(), EscColorCode.size(), NULL, NULL);
                WriteConsole(StdOutHandle, Message.c_str(), Message.size(), NULL, NULL);
                WriteConsole(StdOutHandle, ResetEscColorCode.data(), ResetEscColorCode.size(), NULL, NULL);
            }
            else
            {
                WriteConsole(StdOutHandle, Message.c_str(), Message.size(), NULL, NULL);
            }
        }

    private:
        static constexpr std::string_view LogLevelToEscColorCode(LogLevel Level)
        {
            switch (Level)
            {
            case LogLevel::Critical:
                return "\33[31m"; // Dark Red
            case LogLevel::Error:
                return "\33[31;1m"; // Bold/Bright Red
            case LogLevel::Warning:
                return "\33[33;1m"; // Bold/Bright Yellow
            case LogLevel::Info:
                return "\33[37;1m"; // Bold/Bright White
            case LogLevel::Debug:
                return "\33[36;1m"; // Bold/Bright Aqua
            default:
                return "\33[32;1m"; // Bold/Bright Green
            }
        }

        static constexpr std::string_view ResetEscColorCode = "\33[0m";

        bool ColorsEnabled = false;
        HANDLE StdOutHandle = INVALID_HANDLE_VALUE;
    };

    struct LoggerFile
    {
        ~LoggerFile()
        {
            CloseHandle(FileHandle);
        }

        void Setup()
        {
            auto FilePath = Config::GetFolder() / std::format("{:%Y-%m-%d_%H-%M-%S}.log", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
            FileHandle = CreateFile2(FilePath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, NULL);
            Ensure<LogLevel::Error>(FileHandle != INVALID_HANDLE_VALUE, "Could not open {} for logging", [&] { return std::make_format_args(FilePath); });
        }

        void Log(LogLevel Level, const std::string& Message)
        {
            if (FileHandle != INVALID_HANDLE_VALUE)
            {
                WriteFile(FileHandle, Message.c_str(), Message.size(), NULL, NULL);
            }
        }

    private:
        HANDLE FileHandle = INVALID_HANDLE_VALUE;
    };

    template <class... LoggerTs>
    struct LoggerComposite
    {
        void Setup()
        {
            std::apply([&](auto&... Loggers) { (Loggers.Setup(), ...); }, Loggers);
        }

        void Log(LogLevel Level, const std::string& Message)
        {
            std::apply([&](auto&... Loggers) { (Loggers.Log(Level, Message), ...); }, Loggers);
        }

    private:
        std::tuple<LoggerTs...> Loggers {};
    };

    static auto& GetLogger()
    {
        static LoggerComposite<LoggerConsole, LoggerFile> Logger;
        return Logger;
    }

    void Detail::Abort(const std::string& Message)
    {
        LogRaw(LogLevel::Critical, Message);
    }

    void Detail::LogRaw(LogLevel Level, const std::string& Message)
    {
        GetLogger().Log(Level, Message);
    }

    void LogSetup()
    {
        GetLogger().Setup();
    }
}