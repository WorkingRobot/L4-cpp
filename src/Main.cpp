#include <openssl/provider.h>
#include <streams/FileStream.h>
#include <web/LauncherClient.h>
#include <web/SteamClient.h>
#include <web/zipatch/PatchFile.h>

namespace L4
{
    void ParsePatch(const std::filesystem::path& Path)
    {
        printf("Parsing %s\n", Path.string().c_str());
        FileStream Stream(Path, FileStream::OpenMode::Read, FileStream::CreateMode::OpenOnly, FileStream::ShareMode::Read);
        Plugin::FFXIV::ZiPatch::PatchFile Patch(Stream);
        Plugin::FFXIV::ZiPatch::PatchCtx Ctx {};
        do
        {
            auto Chunk = Patch.ReadChunk();

            if (Chunk)
            {
                Chunk->Apply(Ctx);
            }
        } while (!Patch.AtEOF());
        printf("\n");
    }

    void Main()
    {
        OSSL_PROVIDER_set_default_search_path(NULL, ".");
        OSSL_PROVIDER_load(NULL, "default");
        OSSL_PROVIDER_load(NULL, "legacy");

        static constexpr int n = 1;
        if constexpr (n == 0)
        {
            auto Patchlist = Plugin::FFXIV::LauncherClient().GetBootPatchList(std::string(Plugin::FFXIV::LauncherClient::BaseGameVersion));
        }
        else if constexpr (n == 1)
        {
            auto ret = Plugin::FFXIV::SteamClient(39210).GetTicket().and_then([](const Plugin::FFXIV::Models::SteamTicket& Ticket) {
                return Plugin::FFXIV::LauncherClient().Login("USER", "PASS", "OTP", Ticket, false);
            });

            if (ret)
            {
                printf("%s\n", ret->SessionId.c_str());
            }
        }
        else if constexpr (n == 2)
        {
            auto err = freopen_s((FILE**)stdout, "J:\\misc cold storage\\patches\\ex4\\patches.log", "w", stdout);
            //auto err = freopen_s((FILE**)stdout, "NUL", "w", stdout);
            //setbuf(stdout, NULL);

            for (const auto& File : std::filesystem::directory_iterator("J:\\misc cold storage\\patches\\ex4"))
            {
                if (File.path().extension() != ".patch")
                {
                    continue;
                }
                ParsePatch(File);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}