#pragma once

#include "Debug.h"
#include "Log.h"
#include "utils/Config.h"

namespace L4
{
    static constexpr bool UseConsole = true;
    static constexpr bool CanCreateUIDialogs = true;

    template <class T>
    concept Logger = std::default_initializable<T> &&
                     requires(T t, LogLevel Level, const std::string& Message) {
                         {
                             t.Log(Level, Message)
                             } -> std::same_as<void>;
                     };

    struct LoggerConsole;
    struct LoggerFile;

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

    static std::filesystem::path GetLogFilePath()
    {
        return Config::GetFolder() / fmt::format("{:%Y-%m-%d_%H-%M-%S}.log", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
    }

    static std::filesystem::path GetDumpFilePath()
    {
        return GetLogFilePath().replace_extension("dmp");
    }

    template <Logger... LoggerTs>
    struct LoggerComposite
    {
        void Log(LogLevel Level, const std::string& Message)
        {
            std::apply([&](auto&... Loggers) { (Loggers.Log(Level, Message), ...); }, Loggers);
        }

    private:
        std::tuple<LoggerTs...> Loggers {};
    };

    void LogMiniDump(const std::filesystem::path& DumpPath, bool DumpWritten)
    {
        Detail::LogRaw(LogLevel::Critical, Debug::GetStackTrace());
        Detail::LogRaw(LogLevel::Critical, DumpWritten ? fmt::format("\nDump written to {:s}\n", DumpPath)
                                                       : fmt::format("\nCould not write dump to {:s}\n", DumpPath));
    }
}