#include "LogImpl.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    struct LoggerConsole
    {
        LoggerConsole()
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
        bool ColorsEnabled = false;
        HANDLE StdOutHandle = INVALID_HANDLE_VALUE;
    };

    struct LoggerFile
    {
        LoggerFile()
        {
            auto FilePath = GetLogFilePath();
            FileHandle = CreateFile2(FilePath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, NULL);
            Ensure<LogLevel::Error>(FileHandle != INVALID_HANDLE_VALUE, "Could not open {} for logging", [&] { return std::make_format_args(FilePath); });
        }

        ~LoggerFile()
        {
            CloseHandle(FileHandle);
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

    static Logger auto& GetLogger()
    {
        static LoggerComposite<LoggerConsole, LoggerFile> Logger;
        return Logger;
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

    LONG WINAPI Debug::ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
    {
        if (Debug::IsDebuggerPresent())
        {
            Detail::LogRaw(LogLevel::Critical, PrettifySEHException(ExceptionInfo->ExceptionRecord));
            Detail::LogRaw(LogLevel::Critical, GetStackTrace(ExceptionInfo->ContextRecord));
        }
        else
        {
            auto DumpPath = GetDumpFilePath();
            bool DumpWritten = Debug::WriteMiniDump(DumpPath, ExceptionInfo);

            LogMiniDump(DumpPath, DumpWritten);
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }
}

#include "LogImpl2.h"
