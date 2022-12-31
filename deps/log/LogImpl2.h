#include "LogImpl.h"

namespace L4
{
    static Logger auto& GetLogger()
    {
        static LoggerComposite<LoggerConsole, LoggerFile> Logger;
        return Logger;
    }

    void Detail::Abort(const std::string& Message)
    {
        LogRaw(LogLevel::Critical, Message);

        if (Debug::IsDebuggerPresent())
        {
            LogRaw(LogLevel::Critical, Debug::GetStackTrace());
        }
        else
        {
            auto DumpPath = GetDumpFilePath();
            bool DumpWritten = Debug::WriteMiniDump(DumpPath, nullptr);

            LogMiniDump(DumpPath, DumpWritten);

            Debug::ExitProcess();
        }
    }

    void Detail::LogRaw(LogLevel Level, const std::string& Message)
    {
        GetLogger().Log(Level, Message);
    }

    void SetupLogging()
    {
        GetLogger();
    }
}