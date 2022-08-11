#pragma once

#include <string>
#include <span>
#include <filesystem>

namespace L4
{
    std::string GetStackTrace(void* ContextRecord);

    std::string GetStackTrace(std::span<void*> Frames);

    std::string GetStackTrace();

    bool WriteMiniDump(const std::filesystem::path& Path, void* ExceptionPointers);
}