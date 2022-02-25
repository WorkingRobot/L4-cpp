#include "MBR.h"

#include "../utils/Random.h"

namespace L4
{
    struct CHS
    {
        uint8_t Data[3];

        CHS() = default;

        CHS(uint8_t A, uint8_t B, uint8_t C) :
            Data{ A, B, C }
        {

        }

        CHS(uint64_t LBA)
        {
            static constexpr uint32_t SectorsPerTrack = 63;
            static constexpr uint32_t HeadsPerCylinder = 255;
            uint32_t C = LBA / (HeadsPerCylinder * SectorsPerTrack);
            uint32_t H = (LBA / SectorsPerTrack) % HeadsPerCylinder;
            uint32_t S = (LBA % SectorsPerTrack) + 1;

            if (1023 < C)
                C = 1023, H = 254, S = 63;

            Data[0] = H;
            Data[1] = (S & 0x3F) | ((C >> 2) & 0xC0);
            Data[2] = C & 0xFF;
        }
    };

    struct MBRPartitionPrivate
    {
        uint8_t IsActive;
        CHS FirstCHS;
        uint8_t Type;
        CHS LastCHS;
        uint32_t FirstSectorLBA;
        uint32_t SectorCount;
    };
    static_assert(sizeof(MBRPartitionPrivate) == 16, "MBR partition entry must be 16 bytes long");

#pragma pack(push, 1)
    struct MBRPrivate
    {
        char Boot[440];
        uint32_t DiskSignature;
        uint16_t Reserved;
        MBRPartitionPrivate Partitions[4];
        uint16_t BootSignature;
    };
#pragma pack(pop)
    static_assert(sizeof(MBRPrivate) == sizeof(MBR), "MBR internal representation must be the same size as the public view");
    static_assert(sizeof(MBR) == 512, "MBR must be 512 bytes");

    MBR CreateMBR(const MBRPartition* Partitions, size_t PartitionCount)
    {
        MBRPrivate Data{
            .DiskSignature = Random<uint32_t>(),
            .BootSignature = 0xAA55
        };
        for (int i = 0; i < PartitionCount; ++i)
        {
            Data.Partitions[i] = {
                .FirstCHS = Partitions[i].BlockAddress,
                .Type = Partitions[i].Type,
                .LastCHS = uint64_t(Partitions[i].BlockAddress) + Partitions[i].BlockCount,
                .FirstSectorLBA = Partitions[i].BlockAddress,
                .SectorCount = Partitions[i].BlockCount
            };
        }
        return *(MBR*)&Data;
    }
}