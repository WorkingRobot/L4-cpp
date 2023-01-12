#include "SteamClient.h"

#include <openssl/evp.h>
#include <steam/steam_api.h>
#include <utils/Base64.h>
#include <utils/Align.h>
#include <openssl/err.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace L4::Plugin::FFXIV
{
    using Web::Http::Response;

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/c5580e2a3427b7161ebfa82201056cf54518c073/src/XIVLauncher.Common.Windows/WindowsSteam.cs#L107
    // https://github.com/Facepunch/Facepunch.Steamworks/blob/f91eee68a9083d0353eed7e807eac6ca67b1fe64/Facepunch.Steamworks/SteamClient.cs#L18
    // https://github.com/Facepunch/Facepunch.Steamworks/blob/47b9aca324ca490f7f25ca4e5a2856b31068a698/Facepunch.Steamworks/Classes/SteamApi.cs#L16
    SteamClient::SteamClient(uint32_t AppId) :
        Initialized(false)
    {
        SetEnvironmentVariableA("SteamAppId", std::to_string(AppId).c_str());
        SetEnvironmentVariableA("SteamGameId", std::to_string(AppId).c_str());
        if (!SteamAPI_Init())
        {
            throw SteamException("SteamApi_Init returned false. Steam isn't running, couldn't find Steam, App ID is ureleased, Don't own App ID.");
        }

        Initialized = true;
    }

    SteamClient::~SteamClient()
    {
        if (Initialized)
        {
            SteamAPI_Shutdown();
        }
    }

    Response<Models::SteamTicket> SteamClient::GetTicket()
    {
        try
        {
            return GetTicketInternal();
        }
        catch (const SteamException& Exception)
        {
            return Response<Models::SteamTicket>(std::unexpect, Web::Http::Error { Web::Http::ErrorType::BadHttpCode, Exception.what() });
        }
        catch (const std::runtime_error& Exception)
        {
            return Response<Models::SteamTicket>(std::unexpect, Web::Http::Error { Web::Http::ErrorType::BadFormat, Exception.what() });
        }
    }

    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/c5580e2a3427b7161ebfa82201056cf54518c073/src/XIVLauncher.Common/Encryption/Ticket.cs#L28
    // https://github.com/goatcorp/FFXIVQuickLauncher/blob/c5580e2a3427b7161ebfa82201056cf54518c073/src/XIVLauncher.Common/Encryption/Ticket.cs#L38
    Models::SteamTicket SteamClient::GetTicketInternal()
    {
        std::string Ticket;
        Ticket.resize_and_overwrite(1024, [](char* Buffer, size_t BufferSize) {
            uint32_t TicketSize = 0;
            HAuthTicket Handle = SteamUser()->GetAuthSessionTicket(Buffer, BufferSize, &TicketSize);
            if (Handle == k_HAuthTicketInvalid)
            {
                throw SteamException("Could not get auth session ticket.");
            }
            return TicketSize;
        });

        return EncryptTicket(std::as_bytes(std::span(Ticket.c_str(), Ticket.size())), SteamUtils()->GetServerRealTime());
    }

    Models::SteamTicket SteamClient::EncryptTicket(std::span<const std::byte> Ticket, uint32_t Time)
    {
        Time -= 5;
        Time -= Time % 60;

        std::string HexTicket = fmt::format("{:02x}", fmt::join(Ticket, ""));

        std::string EncodedTicket;
        {
            EncodedTicket.resize_and_overwrite(Align<8>(HexTicket.size() + 1), [&](char* Ptr, size_t Count) {
                auto BeginPtr = Ptr;
                auto EndPtr = Ptr + Count;

                auto TicketSum = std::accumulate((uint8_t*)HexTicket.c_str(), (uint8_t*)HexTicket.c_str() + HexTicket.size(), int16_t(0));
                *(int16_t*)Ptr = TicketSum;
                Ptr += 2;

                HexTicket.copy(Ptr, HexTicket.size());
                Ptr += HexTicket.size();
                *Ptr++ = '\0';

                // Portable implementation of Microsoft's CRT srand() and rand()
                auto Rand = [Seed = Time ^ int(TicketSum)]() mutable {
                    Seed = Seed * 0x343FD + 0x269EC3;
                    return (Seed >> 16 & 0xFFFF) & 0x7FFF;
                };

                auto Checksum = *(uint32_t*)EncodedTicket.c_str();
                std::generate(Ptr, EndPtr, [&]() {
                    static constexpr uint8_t ChecksumLUT[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";

                    uint8_t NextChar = ChecksumLUT[(Checksum + Rand()) & 0x3F];
                    Checksum += NextChar;

                    return NextChar;
                });

                *(uint32_t*)BeginPtr = Checksum;
                std::swap(BeginPtr[0], BeginPtr[1]);

                return Count;
            });
        }

        std::string EncryptedTicket;
        EncryptedTicket.resize_and_overwrite(EncodedTicket.size(), [&](char* Ptr, size_t Count) {
            std::string BlowfishKey = std::format("{:08x}#un@e=x>", Time);

            std::unique_ptr<EVP_CIPHER, decltype([](EVP_CIPHER* Cipher) { return EVP_CIPHER_free(Cipher); })> BlowfishCipher(EVP_CIPHER_fetch(NULL, "BF-ECB", NULL));

            std::unique_ptr<EVP_CIPHER_CTX, decltype([](EVP_CIPHER_CTX* Ctx) { return EVP_CIPHER_CTX_free(Ctx); })> Ctx(EVP_CIPHER_CTX_new());
            EVP_CIPHER_CTX_set_padding(Ctx.get(), 0);

            if (!EVP_EncryptInit_ex2(Ctx.get(), BlowfishCipher.get(), (uint8_t*)BlowfishKey.c_str(), NULL, NULL))
            {
                throw std::runtime_error("Could not init encryption context for Blowfish ECB.");
            }

            int EncLen = 0;
            if (!EVP_EncryptUpdate(Ctx.get(), (uint8_t*)Ptr, &EncLen, (uint8_t*)EncodedTicket.data(), EncodedTicket.size()))
            {
                throw std::runtime_error("Could not EncryptUpdate with Blowfish ECB.");
            }

            int EncLenEnd = 0;
            if (!EVP_EncryptFinal_ex(Ctx.get(), (uint8_t*)Ptr + EncLen, &EncLenEnd))
            {
                throw std::runtime_error("Could not EncryptFinal with Blowfish ECB.");
            }
            EncLen += EncLenEnd;

            return EncLen;
        });

        auto ToSqExBase64 = [](const std::string& Data) {
            auto Ret = B64Encode(std::as_bytes(std::span(Data.c_str(), Data.size())));
            std::ranges::transform(Ret, Ret.begin(), [](char Val) {
                switch (Val)
                {
                case '+':
                    return '-';
                case '/':
                    return '_';
                case '=':
                    return '*';
                default:
                    return Val;
                }
            });
            return Ret;
        };
        EncryptedTicket = ToSqExBase64(EncryptedTicket);

        std::string SplitEncryptedTicket = EncryptedTicket | std::views::chunk(300) | std::views::join_with(',') | std::ranges::to<std::string>();
        return Models::SteamTicket {
            .Text = SplitEncryptedTicket,
            .Length = EncryptedTicket.size()
        };
    }
}