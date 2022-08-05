#include "utils/Log.h"
#include <L4/discord/PresenceConnection.h>

std::unique_ptr<L4::Discord::PresenceConnection> Conn;
void OnReady(void* Ctx, const L4::Discord::User& User)
{
    printf("Ready %s#%s (%s)\n", User.username.c_str(), User.discriminator.c_str(), User.userId.c_str());
    Conn->UpdatePresence(L4::Discord::RichPresence {
        .details = "Manually implemented Discord RPC",
        .state = "was not fun, but i did it anyways",
        .startTimestamp = 1507665886,
        .endTimestamp = 1507665886,
        .partyId = "ae488379-351d-4a4f-ad32-2b9b01c91657",
        .partySize = 1,
        .partyMax = 5,
        .joinSecret = "amongusss",
        //.joinSecret = "MTI4NzM0OjFpMmhuZToxMjMxMjM=",
        .instance = 1 });
}

void OnDisconnected(void* Ctx, int ErrorCode, const std::string& ErrorMessage)
{
    printf("Disconnected %d - %s\n", ErrorCode, ErrorMessage.c_str());
}

void OnError(void* Ctx, int ErrorCode, const std::string& ErrorMessage)
{
    printf("Error %d - %s\n", ErrorCode, ErrorMessage.c_str());
}
void OnJoinGame(void* Ctx, const std::string& JoinSecret)
{
    printf("Join Game %s\n", JoinSecret.c_str());
}
void OnSpectateGame(void* Ctx, const std::string& SpectateSecret)
{
    printf("Spectate Game %s\n", SpectateSecret.c_str());
}
void OnJoinRequest(void* Ctx, const L4::Discord::User& User)
{
    printf("Join Request %s#%s (%s)\n", User.username.c_str(), User.discriminator.c_str(), User.userId.c_str());
}

namespace L4
{
    void Main()
    {
        LogSetup();
        Conn = std::make_unique<L4::Discord::PresenceConnection>("756183418978566307", L4::Discord::PresenceConnection::CallbackList { nullptr, &OnReady, &OnDisconnected, &OnError, &OnJoinGame, &OnSpectateGame, &OnJoinRequest });
        while (true)
        {
            Conn->Tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}