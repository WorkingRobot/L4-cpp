#pragma once

#include <span>
#include <string>

namespace L4
{
    std::string B64Encode(std::span<const std::byte> Input);
}