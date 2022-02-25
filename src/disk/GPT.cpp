#include "GPT.h"

#include "../utils/Align.h"
#include "../utils/Crc32.h"
#include "../utils/Random.h"

#include <utility>

namespace L4
{
    struct Guid
    {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
    };

    struct GPTPartitionPrivate
    {
        Guid Type;
        Guid Id;
        uint64_t Start;
        uint64_t End;
        uint64_t Flags;
        wchar_t Name[36];
    };
    static_assert(sizeof(GPTPartitionPrivate) == 128, "GPT partition entry must be 16 bytes long");

#pragma pack(push, 1)
    struct GPTHeaderPrivate
    {
        uint64_t Magic;
        uint32_t Version;
        uint32_t HeaderLength;
        uint32_t HeaderSum;
        uint32_t Pad;
        uint64_t ThisHeader;
        uint64_t OtherHeader;
        uint64_t DataStart;
        uint64_t DataEnd;
        Guid Guid;
        uint64_t FirstEntry;
        uint32_t EntryCount;
        uint32_t EntryLength;
        uint32_t PartSum;
    };
#pragma pack(pop)
    static_assert(sizeof(GPTHeaderPrivate) == sizeof(GPTHeader), "GPT header internal representation must be the same size as the public view");
    static_assert(sizeof(GPTHeader) == 92, "GPT header must be 92 bytes");

    constexpr uint64_t GPTPartitionCount = 128;
    struct GPTTablePrivate
    {
        GPTPartitionPrivate Partitions[GPTPartitionCount];
    };
    
    constexpr uint64_t GPTMagic = 0x5452415020494645;
    constexpr uint64_t GPTVersion1 = 0x10000;
    constexpr Guid MsDataGuid{ 0xEBD0A0A2, 0x4433B9E5, 0xB668C087, 0xC79926B7 };
    constexpr Guid MsReservedGuid{ 0xE3C9E316, 0x4DB80B5C, 0x2DF97D81, 0xAE1502F0 };
    constexpr Guid LinuxHomeGuid{ 0x933AC7E1, 0x4F132EB4, 0x140E44B8, 0x15F9AEE2 };
    constexpr Guid NullGuid{};

    GPTData CreateGPT(const GPTPartition* Partitions, size_t PartitionCount, size_t BlockSize, size_t DiskBlockCount)
    {
        MBRPartition ProtectiveMBRPartition{
            .BlockAddress = 1,
            .BlockCount = (uint32_t)0xFFFFFFFF,//std::min<size_t>(DiskBlockCount - 1, ~0u - 1),
            .Type = 0xEE
        };

        GPTTablePrivate Table{};

        for (int i = 0; i < PartitionCount; ++i)
        {
            Table.Partitions[i] = {
                .Type = MsDataGuid,
                .Start = Partitions[i].BlockAddress,
                .End = Partitions[i].BlockAddress + Partitions[i].BlockCount - 1,
                .Name = L"Basic data partition"
            };
            RandomGuid((char*)&Table.Partitions[i].Id);
        }

        uint32_t TableChecksum = Crc32Large((const char*)&Table, sizeof(Table));

        GPTHeaderPrivate PrimaryHeader{
            .Magic = GPTMagic,
            .Version = GPTVersion1,
            .HeaderLength = sizeof(GPTHeaderPrivate),
            .ThisHeader = 1,
            .OtherHeader = DiskBlockCount - 1,
            .DataStart = 2 + Align(sizeof(GPTTable), BlockSize) / BlockSize,
            .DataEnd = DiskBlockCount - 1 - Align(sizeof(GPTTable), BlockSize) / BlockSize - 1,
            .FirstEntry = 2,
            .EntryCount = GPTPartitionCount,
            .EntryLength = sizeof(GPTPartitionPrivate),
            .PartSum = TableChecksum
        };
        RandomGuid((char*)&PrimaryHeader.Guid);

        GPTHeaderPrivate SecondaryHeader = PrimaryHeader;
        std::swap(SecondaryHeader.ThisHeader, SecondaryHeader.OtherHeader);
        SecondaryHeader.FirstEntry = SecondaryHeader.DataEnd + 1;

        PrimaryHeader.HeaderSum = Crc32Large((const char*)&PrimaryHeader, sizeof(PrimaryHeader));
        SecondaryHeader.HeaderSum = Crc32Large((const char*)&SecondaryHeader, sizeof(SecondaryHeader));

        return {
            .ProtectiveMBR = CreateMBR(&ProtectiveMBRPartition, 1),
            .PrimaryHeader = *(GPTHeader*)&PrimaryHeader,
            .SecondaryHeader = *(GPTHeader*)&SecondaryHeader,
            .Table = *(GPTTable*)&Table
        };
    }
}