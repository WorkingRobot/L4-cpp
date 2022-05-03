#pragma once

#include "../utils/Align.h"
#include "../utils/Crc32.h"
#include "../utils/Guid.h"
#include "../utils/Random.h"

#include "MBR.h"

namespace L4::Disk
{
#pragma pack(push, 1)
    struct GPT
    {
        struct Header
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

            std::span<const std::byte, 92> AsBytes() const noexcept
            {
                return std::as_bytes(std::span<const Header, 1>(this, 1));
            }
        };
        static_assert(sizeof(Header) == 92);

        static constexpr Guid MsDataGuid { 0xEBD0A0A2, 0x4433B9E5, 0xB668C087, 0xC79926B7 };
        static constexpr Guid MsReservedGuid { 0xE3C9E316, 0x4DB80B5C, 0x2DF97D81, 0xAE1502F0 };
        static constexpr Guid LinuxHomeGuid { 0x933AC7E1, 0x4F132EB4, 0x140E44B8, 0x15F9AEE2 };
        static constexpr Guid NullGuid {};

        struct Partition
        {
            Partition(const L4::Disk::Partition& Part) :
                Type(MsDataGuid),
                Id(RandomGuid()),
                Start(Part.BlockAddress),
                End(Part.BlockAddress + Part.BlockCount - 1),
                Flags(),
                Name { u"Basic data partition" }
            {
            }

            constexpr Partition() = default;

            Guid Type;
            Guid Id;
            uint64_t Start;
            uint64_t End;
            uint64_t Flags;
            std::array<char16_t, 36> Name;
        };
        static_assert(sizeof(Partition) == 128);

        static constexpr uint64_t Magic = 0x5452415020494645;
        static constexpr uint64_t Version1 = 0x10000;
        static constexpr MBR BaseMBR = MBR(std::initializer_list<L4::Disk::Partition> { { .BlockAddress = 1, .BlockCount = 0xFFFFFFFFu, .Type = 0xEE } });

        GPT(std::span<const L4::Disk::Partition> Partitions, uint64_t BlockSize, uint64_t DiskBlockCount) :
            ProtectiveMBR(BaseMBR),
            PrimaryHeader(),
            SecondaryHeader(),
            Table()
        {
            ProtectiveMBR.DiskSignature = Random<uint32_t>();

            if (Partitions.size() > Table.size())
            {
                Partitions = Partitions.first(Table.size());
            }

            std::ranges::copy(Partitions, Table.begin());

            PrimaryHeader = {
                .Magic = Magic,
                .Version = Version1,
                .HeaderLength = sizeof(Header),
                .ThisHeader = 1,
                .OtherHeader = DiskBlockCount - 1,
                .DataStart = 2 + Align(sizeof(Table), BlockSize) / BlockSize,
                .DataEnd = DiskBlockCount - 1 - Align(sizeof(Table), BlockSize) / BlockSize - 1,
                .Guid = RandomGuid(),
                .FirstEntry = 2,
                .EntryCount = uint32_t(Table.size()),
                .EntryLength = sizeof(Partition),
                .PartSum = Crc32(std::span(Table))
            };

            SecondaryHeader = PrimaryHeader;
            std::swap(SecondaryHeader.ThisHeader, SecondaryHeader.OtherHeader);
            SecondaryHeader.FirstEntry = SecondaryHeader.DataEnd + 1;

            PrimaryHeader.HeaderSum = Crc32(std::span(&PrimaryHeader, 1));
            SecondaryHeader.HeaderSum = Crc32(std::span(&SecondaryHeader, 1));
        }

        MBR ProtectiveMBR;
        Header PrimaryHeader;
        Header SecondaryHeader;
        std::array<Partition, 128> Table;
    };
#pragma pack(pop)
}