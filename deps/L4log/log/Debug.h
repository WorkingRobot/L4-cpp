#pragma once

#include <filesystem>
#include <span>
#include <string>

namespace L4::Debug
{
    std::string GetStackTrace(void* ContextRecord);

    std::string GetStackTrace(std::span<void*> Frames);

    std::string GetStackTrace();

    bool IsDebuggerPresent();

    [[noreturn]] void ExitProcess(int ReturnCode = 0);

    bool WriteMiniDump(const std::filesystem::path& Path, void* ExceptionPointers);

    void SetupExceptions();
}