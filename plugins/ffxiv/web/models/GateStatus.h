#pragma once

#include <web/JsonMacros.h>

namespace L4::Plugin::FFXIV::Models
{
    struct GateStatus
    {
        bool Status;
        std::vector<std::u8string> Message;
        std::vector<std::u8string> News;

        JSON_DEFINE
        {
            JSON_ITEM("status", Status);
            JSON_ITEM_OPT("message", Message);
            JSON_ITEM_OPT("news", News);
        }
    };
}