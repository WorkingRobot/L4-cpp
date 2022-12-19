#pragma once

#include <string>

namespace L4::Plugin::FFXIV::Models
{
    struct SteamTicket
    {
        std::string Text;
        size_t Length;
    };
}