#pragma once

#include <string>

namespace L4
{
    std::string GetStackTrace();

    std::string GetStackTrace(void* ContextRecord);
}