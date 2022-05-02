#include "GPT.h"

#include "../utils/Align.h"
#include "../utils/Crc32.h"
#include "../utils/Random.h"

#include <utility>

namespace L4::Disk::GPT
{
    struct PartitionPrivate
    {
        Guid Type;
        Guid Id;
        uint64_t Start;
        uint64_t End;
        uint64_t Flags;
        std::array<char16_t, 36> Name;
    };
    static_assert(sizeof(PartitionPrivate) == 128, "GPT partition entry must be 16 bytes long");

#pragma pack(push, 1)
    struct HeaderPrivate
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
    static_assert(sizeof(HeaderPrivate) == 92, "GPT header internal representation must be 92 bytes");

    constexpr uint64_t MaxPartitionCount = 128;
    using TablePrivate = std::array<PartitionPrivate, MaxPartitionCount>;

    constexpr uint64_t Magic = 0x5452415020494645;
    constexpr uint64_t Version1 = 0x10000;
    constexpr Guid MsDataGuid { 0xEBD0A0A2, 0x4433B9E5, 0xB668C087, 0xC79926B7 };
    constexpr Guid MsReservedGuid { 0xE3C9E316, 0x4DB80B5C, 0x2DF97D81, 0xAE1502F0 };
    constexpr Guid LinuxHomeGuid { 0x933AC7E1, 0x4F132EB4, 0x140E44B8, 0x15F9AEE2 };
    constexpr Guid NullGuid {};

    GPT Create(uint64_t BlockSize, uint64_t DiskBlockCount, const Partition* Partitions, uint8_t PartitionCount)
    {
        if (PartitionCount > 128)
        {
            PartitionCount = 128;
        }

        MBR::Partition ProtectiveMBRPartition {
            .BlockAddress = 1,
            .BlockCount = 0xFFFFFFFFu,
            .Type = 0xEE
        };

        TablePrivate TablePriv {};

        for (uint8_t Idx = 0; Idx < PartitionCount; ++Idx)
        {
            TablePriv[Idx] = PartitionPrivate {
                .Type = MsDataGuid,
                .Id = RandomGuid(),
                .Start = Partitions[Idx].BlockAddress,
                .End = Partitions[Idx].BlockAddress + Partitions[Idx].BlockCount - 1,
                .Name = { u"Basic data partition" }
            };
        }

        uint32_t TableChecksum = Crc32Large((const char*)&TablePriv, sizeof(Table));

        HeaderPrivate PrimaryHeader {
            .Magic = Magic,
            .Version = Version1,
            .HeaderLength = sizeof(HeaderPrivate),
            .ThisHeader = 1,
            .OtherHeader = DiskBlockCount - 1,
            .DataStart = 2 + Align(sizeof(Table), BlockSize) / BlockSize,
            .DataEnd = DiskBlockCount - 1 - Align(sizeof(Table), BlockSize) / BlockSize - 1,
            .Guid = RandomGuid(),
            .FirstEntry = 2,
            .EntryCount = MaxPartitionCount,
            .EntryLength = sizeof(PartitionPrivate),
            .PartSum = TableChecksum
        };

        HeaderPrivate SecondaryHeader = PrimaryHeader;
        std::swap(SecondaryHeader.ThisHeader, SecondaryHeader.OtherHeader);
        SecondaryHeader.FirstEntry = SecondaryHeader.DataEnd + 1;

        PrimaryHeader.HeaderSum = Crc32Large((const char*)&PrimaryHeader, sizeof(PrimaryHeader));
        SecondaryHeader.HeaderSum = Crc32Large((const char*)&SecondaryHeader, sizeof(SecondaryHeader));

        return {
            .ProtectiveMBR = MBR::Create(&ProtectiveMBRPartition, 1),
            .PrimaryHeader = *(Header*)&PrimaryHeader,
            .SecondaryHeader = *(Header*)&SecondaryHeader,
            .Table = *(Table*)&TablePriv
        };
    }
}