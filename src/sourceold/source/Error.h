#pragma once

#include <cstdint>

namespace L4::Interface
{
    enum class Error : uint16_t
    {
        Success,

        UnsupportedArchive,
        UnsupportedOperation,
        StreamGuidMismatch,
        StreamVersionMismatch,
        StreamElementSizeMismatch,
        RecursiveDirectoryTree,

        Unauthenticated,
        InvalidAuthentication,
        FailedHttpRequest,
    };
}