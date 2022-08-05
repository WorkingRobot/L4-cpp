#pragma once

#include "RpcConnection.h"
#include "Structs.h"

namespace L4::Discord
{
    enum class JoinReply : int32_t
    {
        No,
        Yes,
        Ignore
    };

    class PresenceConnection
    {
    public:
        struct CallbackList
        {
            void* Ctx;

            void (*OnReady)(void* Ctx, const User& User);
            void (*OnDisconnected)(void* Ctx, int ErrorCode, const std::string& ErrorMessage);
            void (*OnError)(void* Ctx, int ErrorCode, const std::string& ErrorMessage);
            void (*OnJoinGame)(void* Ctx, const std::string& JoinSecret);
            void (*OnSpectateGame)(void* Ctx, const std::string& SpectateSecret);
            void (*OnJoinRequest)(void* Ctx, const User& User);
        };

        PresenceConnection(const std::string& ApplicationId, CallbackList Callbacks);

        void UpdatePresence(const RichPresence& Presence);

        void ClearPresence();

        void RespondToJoin(const std::string& UserId, JoinReply Reply);

        void Tick();

    private:
        static void OnRpcConnect(void* Ctx, const JsonDocument& Document);
        static void OnRpcDisconnect(void* Ctx, int ErrorCode, const std::string& ErrorMessage);

        void RegisterForEvent(const std::string& EventName);

        void Open();

        void Close();

        RpcConnection Rpc;
        CallbackList Callbacks;
        int Nonce;
    };
}