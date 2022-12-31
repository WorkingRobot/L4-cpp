#pragma once

#include <string>

namespace L4::Discord
{
    enum class PartyPrivacy : uint8_t
    {
        Private,
        Public
    };

    struct User
    {
        std::string userId;
        std::string username;
        std::string discriminator;
        std::string avatar;
    };

    struct RichPresence
    {
        std::string state; /* max 128 bytes */
        std::string details; /* max 128 bytes */
        int64_t startTimestamp;
        int64_t endTimestamp;
        std::string largeImageKey; /* max 32 bytes */
        std::string largeImageText; /* max 128 bytes */
        std::string smallImageKey; /* max 32 bytes */
        std::string smallImageText; /* max 128 bytes */
        std::string partyId; /* max 128 bytes */
        int partySize;
        int partyMax;
        PartyPrivacy partyPrivacy;
        std::string matchSecret; /* max 128 bytes */
        std::string joinSecret; /* max 128 bytes */
        std::string spectateSecret; /* max 128 bytes */
        int8_t instance;
    };
}