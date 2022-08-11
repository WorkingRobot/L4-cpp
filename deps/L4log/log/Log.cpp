#include "Log.h"

#include "utils/Config.h"
#include "utils/Debug.h"
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
        if (IsDebuggerPresent())
        {
            LogRaw(LogLevel::Critical, Message);
            LogRaw(LogLevel::Critical, GetStackTrace());
        }
        else
        {
            auto DumpPath = Config::GetFolder() / std::format("{:%Y-%m-%d_%H-%M-%S}.dmp", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
            bool DumpWritten = WriteMiniDump(DumpPath, NULL);

            LogRaw(LogLevel::Critical, Message);
            LogRaw(LogLevel::Critical, GetStackTrace());
            LogRaw(LogLevel::Critical, DumpWritten ? std::format("\nDump written to {:s}\n", DumpPath)
                                                   : std::format("\nCould not write dump to {:s}\n", DumpPath));

            ExitProcess(0);
        }
    }

    void Detail::LogRaw(LogLevel Level, const std::string& Message)
    {
        GetLogger().Log(Level, Message);
    }

    static constexpr std::string_view GetExceptionNameFromCode(DWORD ExceptionCode)
    {
        switch (ExceptionCode)
        {
            // clang-format off
#define CASE(Name) case Name: return #Name
            // clang-format on

            CASE(EXCEPTION_ACCESS_VIOLATION);
            CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
            CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
            CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
            CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
            CASE(EXCEPTION_FLT_INVALID_OPERATION);
            CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
            CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
            CASE(EXCEPTION_PRIV_INSTRUCTION);
            CASE(EXCEPTION_STACK_OVERFLOW);

#undef CASE
        default:
            return "";
        }
    }

    static inline std::string PrettifySEHException(PEXCEPTION_RECORD Record)
    {
        std::string Ret;
        Ret.reserve(256);
        auto Itr = std::back_inserter(Ret);
        Itr = std::format_to(Itr, "SEH Exception: ");
        if (auto Code = GetExceptionNameFromCode(Record->ExceptionCode); !Code.empty())
        {
            Itr = std::format_to(Itr, "{:s}", Code);
        }
        else
        {
            Itr = std::format_to(Itr, "{:#08x}", Record->ExceptionCode);
        }
        if (Record->NumberParameters)
        {
            *Itr++ = ' ';
            *Itr++ = '[';
            for (DWORD Idx = 0; Idx < Record->NumberParameters; ++Idx)
            {
                if (Idx)
                {
                    *Itr++ = ',';
                    *Itr++ = ' ';
                }
                Itr = std::format_to(Itr, "{:x}", Record->ExceptionInformation[Idx]);
            }
            *Itr++ = ']';
        }
        *Itr++ = '\n';

        return Ret;
    }

    static LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
    {
        if (IsDebuggerPresent())
        {
            Detail::LogRaw(LogLevel::Critical, PrettifySEHException(ExceptionInfo->ExceptionRecord));
            Detail::LogRaw(LogLevel::Critical, GetStackTrace(ExceptionInfo->ContextRecord));
        }
        else
        {
            auto DumpPath = Config::GetFolder() / std::format("{:%Y-%m-%d_%H-%M-%S}.dmp", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
            bool DumpWritten = WriteMiniDump(DumpPath, ExceptionInfo);

            Detail::LogRaw(LogLevel::Critical, PrettifySEHException(ExceptionInfo->ExceptionRecord));
            Detail::LogRaw(LogLevel::Critical, GetStackTrace(ExceptionInfo->ContextRecord));
            Detail::LogRaw(LogLevel::Critical, DumpWritten ? std::format("\nDump written to {:s}\n", DumpPath)
                                                           : std::format("\nCould not write dump to {:s}\n", DumpPath));
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void LogSetup()
    {
        ULONG StackBytes = 0x10000;
        SetThreadStackGuarantee(&StackBytes);
        SetUnhandledExceptionFilter(ExceptionHandler);

        GetLogger().Setup();
    }
}