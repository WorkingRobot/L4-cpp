#pragma once

#include <string>

namespace L4::Plugin::FFXIV::Models
{
    struct LoginResult
    {
        std::string SessionId;
        int Region;
        bool AcceptedToS;
        bool IsPlayable;
        int MaxExpansion;
    };
}