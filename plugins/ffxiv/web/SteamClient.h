#pragma once

#include <web/Client.h>
#include <stdexcept>

#include "models/SteamTicket.h"

namespace L4::Plugin::FFXIV
{
    class SteamException : public std::runtime_error
    {
    public:
        explicit SteamException(const std::string& Message) :
            std::runtime_error(Message.c_str())
        {
        }

        explicit SteamException(const char* Message) :
            std::runtime_error(Message)
        {
        }
    };

    class SteamClient : public Web::Http::Client
    {
    public:
        SteamClient(uint32_t AppId);

        ~SteamClient();

        Response<Models::SteamTicket> GetTicket();

    private:
        Models::SteamTicket GetTicketInternal();
        
        static Models::SteamTicket EncryptTicket(std::span<const std::byte> Ticket, uint32_t Time);

        bool Initialized;
    };
}