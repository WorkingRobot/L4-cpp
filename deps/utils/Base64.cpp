#include "Base64.h"

#include "base64/base64.h"

namespace L4
{
    std::string B64Encode(std::span<const std::byte> Input)
    {
        return base64_encode((const uint8_t*)Input.data(), Input.size());
    }
}