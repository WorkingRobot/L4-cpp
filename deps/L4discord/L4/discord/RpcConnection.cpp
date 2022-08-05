#include "RpcConnection.h"

#include "JsonParsing.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4::Discord
{
    static constexpr int RpcVersion = 1;

    RpcConnection::RpcConnection(const std::string& ApplicationId, CallbackList Callbacks) :
        CurrentState(State::Disconnected),
        ErrorCode(0),
        ErrorMessage(),
        ApplicationId(ApplicationId),
        Callbacks(Callbacks)
    {

    }

    void RpcConnection::Open()
    {
        if (CurrentState == State::Connected)
        {
            return;
        }

        if (CurrentState == State::Disconnected && !Ipc.Connect())
        {
            return;
        }

        if (CurrentState == State::SentHandshake)
        {
            JsonDocument Document;
            if (ReadJson(Document))
            {
                auto Command = Document.GetStringMember("cmd");
                auto Event = Document.GetStringMember("evt");
                if (Command && Event)
                {
                    if (!strcmp(Command, "DISPATCH") && !strcmp(Event, "READY"))
                    {
                        CurrentState = State::Connected;
                        Callbacks.OnConnect(Callbacks.Ctx, Document);
                    }
                }

                auto Data = Document.GetObjectMember("data");
                ErrorCode = Data->GetIntMember("code");
                ErrorMessage = Data->GetStringMember("message", "");

            }
        }
        else
        {
            MessageFrame Frame {};
            Frame.Opcode = Opcode::Handshake;
            JsonWriter Writer(Frame.Message, sizeof(Frame.Message));
            {
                Writer.StartObject();
                {
                    Writer.Key("v", 1);
                    Writer.Int(RpcVersion);
                }
                {
                    Writer.Key("client_id", 9);
                    Writer.String(ApplicationId.c_str(), ApplicationId.size());
                }
                Writer.EndObject();
            }
            Writer.Flush();
            Frame.Length = Writer.Size();
            printf("Written handshake: %s\n", Frame.Message);
            if (Ipc.WriteBytes(&Frame, Frame.HeaderSize + Frame.Length))
            {
                CurrentState = State::SentHandshake;
            }
            else
            {
                Close();
            }
        }
    }

    void RpcConnection::Close()
    {
        if (CurrentState == State::Connected || CurrentState == State::SentHandshake)
        {
            Callbacks.OnDisconnect(Callbacks.Ctx, ErrorCode, ErrorMessage);
        }
        Ipc.Disconnect();
        CurrentState = State::Disconnected;
    }

    bool RpcConnection::WriteFrame(const void* Data, size_t Size)
    {
        MessageFrame Frame {};
        Frame.Opcode = Opcode::Frame;
        Frame.Length = Size;
        memcpy(Frame.Message, Data, Size);
        printf("Written frame: %s\n", Frame.Message);
        if (Ipc.WriteBytes(&Frame, Frame.HeaderSize + Frame.Length))
        {
            return true;
        }
        else
        {
            Close();
            return false;
        }
    }

    bool RpcConnection::ReadJson(JsonDocument& Document)
    {
        if (CurrentState != State::Connected && CurrentState != State::SentHandshake)
        {
            return false;
        }

        // Document.Clear();
        do
        {
            bool ReadSuccessful = Ipc.ReadBytes(&ReadFrame, MessageFrame::HeaderSize);
            if (!ReadSuccessful)
            {
                if (!Ipc.IsOpen())
                {
                    ErrorCode = 0;
                    ErrorMessage = "Pipe closed";
                    Close();
                }
                return false;
            }

            if (ReadFrame.Length > 0)
            {
                ReadSuccessful = Ipc.ReadBytes(ReadFrame.Message, ReadFrame.Length);
                if (!ReadSuccessful)
                {
                    ErrorCode = 0;
                    ErrorMessage = "Partial data in frame";
                    Close();
                    return false;
                }
                ReadFrame.Message[ReadFrame.Length] = '\0';
            }

            switch (ReadFrame.Opcode)
            {
            case Opcode::Frame:
                printf("Recieved frame %d - %s\n", ReadFrame.Length, ReadFrame.Message);
                Document.ParseInsitu(ReadFrame.Message);
                return true;
            case Opcode::Close:
                printf("Recieved close %d - %s\n", ReadFrame.Length, ReadFrame.Message);
                Document.ParseInsitu(ReadFrame.Message);
                ErrorCode = Document.GetIntMember("code");
                ErrorMessage = Document.GetStringMember("message", "");
                Close();
                return false;
            case Opcode::Ping:
                ReadFrame.Opcode = Opcode::Pong;
                printf("Written pong: %s\n", ReadFrame.Message);
                if (!Ipc.WriteBytes(&ReadFrame, ReadFrame.HeaderSize + ReadFrame.Length))
                {
                    Ipc.Disconnect();
                }
                break;
            case Opcode::Pong:
                printf("Recieved pong %d - %s\n", ReadFrame.Length, ReadFrame.Message);
                break;
            case Opcode::Handshake:
                printf("Recieved handshake %d - %s\n", ReadFrame.Length, ReadFrame.Message);
                break;
            default:
                ErrorCode = 0;
                ErrorMessage = "Bad ipc frame";
                Close();
                return false;
            }
        } while (true);
        __assume(false);
    }
}