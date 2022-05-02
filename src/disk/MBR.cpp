#include "MBR.h"

#include "../utils/Random.h"

namespace L4::Disk::MBR
{
    using CHS = std::array<std::byte, 3>;

    CHS CreateCHS(uint64_t LBA)
    {
        static constexpr uint32_t SectorsPerTrack = 63;
        static constexpr uint32_t HeadsPerCylinder = 255;
        uint32_t C = LBA / (HeadsPerCylinder * SectorsPerTrack);
        uint32_t H = (LBA / SectorsPerTrack) % HeadsPerCylinder;
        uint32_t S = (LBA % SectorsPerTrack) + 1;

        if (C > 1023)
        {
            C = 1023;
            H = 254;
            S = 63;
        }

        return { std::byte(H), std::byte((S & 0x3F) | ((C >> 2) & 0xC0)), std::byte(C & 0xFF) };
    }

    struct PartitionPrivate
    {
        uint8_t IsActive;
        CHS FirstCHS;
        uint8_t Type;
        CHS LastCHS;
        uint32_t FirstSectorLBA;
        uint32_t SectorCount;
    };
    static_assert(sizeof(PartitionPrivate) == 16, "MBR partition entry must be 16 bytes long");

#pragma pack(push, 1)
    struct MBRPrivate
    {
        std::array<std::byte, 440> Boot;
        uint32_t DiskSignature;
        uint16_t Reserved;
        std::array<PartitionPrivate, 4> Partitions;
        uint16_t BootSignature;
    };
#pragma pack(pop)
    static_assert(sizeof(MBRPrivate) == 512, "MBR internal representation must be 512 bytes");

    MBR Create(const Partition* Partitions, uint8_t PartitionCount)
    {
        if (PartitionCount > 4)
        {
            PartitionCount = 4;
        }

        MBRPrivate Data {};
        Data.DiskSignature = Random<uint32_t>();
        Data.BootSignature = 0xAA55;

        for (uint8_t Idx = 0; Idx < PartitionCount; ++Idx)
        {
            Data.Partitions[Idx] = PartitionPrivate {
                .FirstCHS = CreateCHS(Partitions[Idx].BlockAddress),
                .Type = Partitions[Idx].Type,
                .LastCHS = CreateCHS(uint64_t(Partitions[Idx].BlockAddress) + Partitions[Idx].BlockCount),
                .FirstSectorLBA = Partitions[Idx].BlockAddress,
                .SectorCount = Partitions[Idx].BlockCount
            };
        }
        return *(MBR*)&Data;
    }
}