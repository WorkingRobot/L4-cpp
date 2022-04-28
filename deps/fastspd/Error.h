#pragma once

#include <system_error>

namespace FastSpd {
    template <class T = std::system_error, class... ArgTs>
    static T CreateErrorWin32(uint32_t Error, ArgTs&&... Args)
    {
        return { (int)Error, std::system_category(), std::forward<ArgTs>(Args)... };
    }
}