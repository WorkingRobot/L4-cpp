#pragma once

#include "IpcConnection.h"
#include "JsonParsing.h"

#include <string>

namespace L4::Discord
{
    class RpcConnection
    {
        struct CallbackList
        {
            void* Ctx;

            void (*OnConnect)(void* Ctx, const JsonDocument& Document);
            void (*OnDisconnect)(void* Ctx, int ErrorCode, const std::string& ErrorMessage);
        };

    public:
        RpcConnection(const std::string& ApplicationId, CallbackList Callbacks);

        void Open();

        bool IsOpen() const noexcept
        {
            return CurrentState == State::Connected;
        }

        bool ReadJson(JsonDocument& Document);

        bool WriteFrame(const void* Data, size_t Size);

    private:
        void Close();

        enum class State : uint8_t
        {
            Disconnected,
            SentHandshake,
            AwaitingResponse,
            Connected
        };

        enum class Opcode : uint32_t
        {
            Handshake,
            Frame,
            Close,
            Ping,
            Pong
        };

        struct MessageFrame
        {
            static constexpr size_t MaxFrameSize = 1 << 16;
            static constexpr size_t HeaderSize = 8;

            // Header
            Opcode Opcode;
            uint32_t Length;

            // Body
            char Message[MaxFrameSize - HeaderSize];
        };
        static_assert(sizeof(MessageFrame) == MessageFrame::MaxFrameSize, "A frame should be equal to the max size it can take up");

        IpcConnection Ipc;
        State CurrentState;
        int ErrorCode;
        std::string ErrorMessage;
        std::string ApplicationId;
        CallbackList Callbacks;
        MessageFrame ReadFrame;
    };
}