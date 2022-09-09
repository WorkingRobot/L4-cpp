#include "PresenceConnection.h"

#include "JsonParsing.h"

namespace L4::Discord
{
    PresenceConnection::PresenceConnection(const std::string& ApplicationId, CallbackList Callbacks) :
        Rpc(ApplicationId, { this, &PresenceConnection::OnRpcConnect, &PresenceConnection::OnRpcDisconnect }),
        Callbacks(Callbacks),
        Nonce(0)
    {
        // Skip registering
    }

    void PresenceConnection::UpdatePresence(const RichPresence& Presence)
    {
        char Message[1 << 14] {};
        JsonWriter Writer(Message, sizeof(Message));
        {
            Writer.StartObject();
            {
                Writer.Key("nonce", 5);
                auto NonceString = std::to_string(Nonce++);
                Writer.String(NonceString.c_str(), NonceString.size());
            }
            {
                Writer.Key("cmd", 3);
                Writer.String("SET_ACTIVITY", 12);
            }
            {
                Writer.Key("args", 4);
                Writer.StartObject();
                {
                    Writer.Key("pid", 3);
                    Writer.Int(IpcConnection::GetProcessId());
                }
                {
                    Writer.Key("activity", 8);
                    Writer.StartObject();
                    if (!Presence.state.empty())
                    {
                        Writer.Key("state", 5);
                        Writer.String(Presence.state.c_str(), Presence.state.size());
                    }
                    if (!Presence.details.empty())
                    {
                        Writer.Key("details", 7);
                        Writer.String(Presence.details.c_str(), Presence.details.size());
                    }
                    if (Presence.startTimestamp || Presence.endTimestamp)
                    {
                        Writer.Key("timestamps", 10);
                        Writer.StartObject();
                        {
                            if (Presence.startTimestamp)
                            {
                                Writer.Key("start", 5);
                                Writer.Int64(Presence.startTimestamp);
                            }
                            if (Presence.endTimestamp)
                            {
                                Writer.Key("end", 3);
                                Writer.Int64(Presence.endTimestamp);
                            }
                        }
                        Writer.EndObject();
                    }
                    if (!Presence.largeImageKey.empty() ||
                        !Presence.largeImageText.empty() ||
                        !Presence.smallImageKey.empty() ||
                        !Presence.smallImageText.empty())
                    {
                        Writer.Key("assets", 6);
                        Writer.StartObject();
                        {
                            if (!Presence.largeImageKey.empty())
                            {
                                Writer.Key("large_image", 11);
                                Writer.String(Presence.largeImageKey.c_str(), Presence.largeImageKey.size());
                            }
                            if (!Presence.largeImageText.empty())
                            {
                                Writer.Key("large_text", 10);
                                Writer.String(Presence.largeImageText.c_str(), Presence.largeImageText.size());
                            }
                            if (!Presence.smallImageKey.empty())
                            {
                                Writer.Key("small_image", 11);
                                Writer.String(Presence.smallImageKey.c_str(), Presence.smallImageKey.size());
                            }
                            if (!Presence.smallImageText.empty())
                            {
                                Writer.Key("small_text", 10);
                                Writer.String(Presence.smallImageText.c_str(), Presence.smallImageText.size());
                            }
                        }
                        Writer.EndObject();
                    }
                    if (!Presence.partyId.empty() ||
                        (Presence.partySize && Presence.partyMax) ||
                        Presence.partyPrivacy != PartyPrivacy::Private)
                    {
                        Writer.Key("party", 5);
                        Writer.StartObject();
                        {
                            if (!Presence.partyId.empty())
                            {
                                Writer.Key("id", 2);
                                Writer.String(Presence.partyId.c_str(), Presence.partyId.size());
                            }
                            if (Presence.partySize && Presence.partyMax)
                            {
                                Writer.Key("size", 4);
                                Writer.StartArray();
                                Writer.Int(Presence.partySize);
                                Writer.Int(Presence.partyMax);
                                Writer.EndArray();
                            }
                            if (Presence.partyPrivacy != PartyPrivacy::Private)
                            {
                                Writer.Key("privacy", 7);
                                Writer.Int(static_cast<int>(Presence.partyPrivacy));
                            }
                        }
                        Writer.EndObject();
                    }
                    if (!Presence.matchSecret.empty() ||
                        !Presence.joinSecret.empty() ||
                        !Presence.spectateSecret.empty())
                    {
                        Writer.Key("secrets", 7);
                        Writer.StartObject();
                        {
                            if (!Presence.matchSecret.empty())
                            {
                                Writer.Key("match", 5);
                                Writer.String(Presence.matchSecret.c_str(), Presence.matchSecret.size());
                            }
                            if (!Presence.joinSecret.empty())
                            {
                                Writer.Key("join", 4);
                                Writer.String(Presence.joinSecret.c_str(), Presence.joinSecret.size());
                            }
                            if (!Presence.spectateSecret.empty())
                            {
                                Writer.Key("spectate", 8);
                                Writer.String(Presence.spectateSecret.c_str(), Presence.spectateSecret.size());
                            }
                        }
                        Writer.EndObject();
                    }
                    {
                        Writer.Key("instance", 8);
                        Writer.Bool(Presence.instance != 0);
                    }
                    Writer.EndObject();
                }
                Writer.EndObject();
            }
            Writer.EndObject();
        }
        Writer.Flush();
        Rpc.WriteFrame(Message, Writer.Size());
    }

    void PresenceConnection::ClearPresence()
    {
        char Message[1 << 14] {};
        JsonWriter Writer(Message, sizeof(Message));
        {
            Writer.StartObject();
            {
                Writer.Key("nonce", 5);
                auto NonceString = std::to_string(Nonce++);
                Writer.String(NonceString.c_str(), NonceString.size());
            }
            {
                Writer.Key("cmd", 3);
                Writer.String("SET_ACTIVITY", 12);
            }
            {
                Writer.Key("args", 4);
                Writer.StartObject();
                {
                    Writer.Key("pid", 3);
                    Writer.Int(IpcConnection::GetProcessId());
                }
                Writer.EndObject();
            }
            Writer.EndObject();
        }
        Writer.Flush();
        Rpc.WriteFrame(Message, Writer.Size());
    }

    void PresenceConnection::RespondToJoin(const std::string& UserId, JoinReply Reply)
    {
        char Message[1 << 14] {};
        JsonWriter Writer(Message, sizeof(Message));
        {
            Writer.StartObject();
            {
                Writer.Key("nonce", 5);
                auto NonceString = std::to_string(Nonce++);
                Writer.String(NonceString.c_str(), NonceString.size());
            }
            {
                Writer.Key("cmd", 3);
                if (Reply == JoinReply::Yes)
                {
                    Writer.String("SEND_ACTIVITY_JOIN_INVITE", 25);
                }
                else
                {
                    Writer.String("CLOSE_ACTIVITY_JOIN_REQUEST", 27);
                }
            }
            {
                Writer.Key("args", 4);
                Writer.StartObject();
                {
                    Writer.Key("user_id", 8);
                    Writer.String(UserId.c_str(), UserId.size());
                }
                Writer.EndObject();
            }
            Writer.EndObject();
        }
        Writer.Flush();
        Rpc.WriteFrame(Message, Writer.Size());
    }

    void PresenceConnection::Tick()
    {
        if (!Rpc.IsOpen())
        {
            Rpc.Open();
            return;
        }

        while (true)
        {
            JsonDocument Document;

            if (!Rpc.ReadJson(Document))
            {
                break;
            }

            auto EventName = Document.GetStringMember("evt");
            auto Nonce = Document.GetStringMember("nonce");

            if (Nonce)
            {
                if (EventName && !strcmp(EventName, "ERROR"))
                {
                    auto Data = Document.GetObjectMember("data");
                    Callbacks.OnError(Callbacks.Ctx, Data->GetIntMember("code"), Data->GetStringMember("message", ""));
                }
            }
            else if (EventName)
            {
                continue;
            }
            else
            {
                auto Data = Document.GetObjectMember("data");

                if (!strcmp(EventName, "ACTIVITY_JOIN"))
                {
                    Callbacks.OnJoinGame(Callbacks.Ctx, Data->GetStringMember("secret"));
                }
                else if (!strcmp(EventName, "ACTIVITY_SPECTATE"))
                {
                    Callbacks.OnSpectateGame(Callbacks.Ctx, Data->GetStringMember("secret"));
                }
                else if (!strcmp(EventName, "ACTIVITY_JOIN_REQUEST"))
                {
                    auto UserData = Data->GetObjectMember("user");
                    User User {
                        .userId = UserData->GetStringMember("id"),
                        .username = UserData->GetStringMember("username"),
                        .discriminator = UserData->GetStringMember("discriminator", ""),
                        .avatar = UserData->GetStringMember("avatar", "")
                    };
                    Callbacks.OnJoinRequest(Callbacks.Ctx, User);
                }
            }
        }
    }

    void PresenceConnection::OnRpcConnect(void* Ctx, const JsonDocument& Document)
    {
        auto Self = static_cast<PresenceConnection*>(Ctx);

        Self->RegisterForEvent("ACTIVITY_JOIN");
        Self->RegisterForEvent("ACTIVITY_SPECTATE");
        Self->RegisterForEvent("ACTIVITY_JOIN_REQUEST");

        auto Data = Document.GetObjectMember("data");
        auto UserData = Data->GetObjectMember("user");
        User User {
            .userId = UserData->GetStringMember("id"),
            .username = UserData->GetStringMember("username"),
            .discriminator = UserData->GetStringMember("discriminator", ""),
            .avatar = UserData->GetStringMember("avatar", "")
        };
        Self->Callbacks.OnReady(Self->Callbacks.Ctx, User);
    }

    void PresenceConnection::OnRpcDisconnect(void* Ctx, int ErrorCode, const std::string& ErrorMessage)
    {
        auto Self = static_cast<PresenceConnection*>(Ctx);

        Self->Callbacks.OnDisconnected(Self->Callbacks.Ctx, ErrorCode, ErrorMessage);
    }

    void PresenceConnection::RegisterForEvent(const std::string& EventName)
    {
        char Message[1 << 14] {};
        JsonWriter Writer(Message, sizeof(Message));
        {
            Writer.StartObject();
            {
                Writer.Key("nonce", 5);
                auto NonceString = std::to_string(Nonce++);
                Writer.String(NonceString.c_str(), NonceString.size());
            }
            {
                Writer.Key("cmd", 3);
                Writer.String("SUBSCRIBE", 9);
            }
            {
                Writer.Key("evt", 3);
                Writer.String(EventName.c_str(), EventName.size());
            }
            Writer.EndObject();
        }
        Writer.Flush();
        Rpc.WriteFrame(Message, Writer.Size());
    }
}