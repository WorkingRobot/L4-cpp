#pragma once

#include <errno.h>

#include <system_error>

namespace L4
{
    template <class T = std::system_error, class... ArgTs>
    static T CreateErrorErrno(ArgTs&&... Args)
    {
        return { errno, std::system_category(), std::forward<ArgTs>(Args)... };
    }

    template <class T = std::system_error, class... ArgTs>
    static T CreateErrorCustom(int ErrorCode, ArgTs&&... Args)
    {
        return { ErrorCode, std::system_category(), std::forward<ArgTs>(Args)... };
    }
}