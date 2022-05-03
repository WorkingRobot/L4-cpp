#pragma once

#include "Partition.h"

#include <array>
#include <span>

namespace L4::Disk
{
    struct CHS
    {
        explicit constexpr CHS(uint64_t LBA) noexcept
        {
            constexpr uint32_t SectorsPerTrack = 63;
            constexpr uint32_t HeadsPerCylinder = 255;
            uint32_t C = LBA / (HeadsPerCylinder * SectorsPerTrack);
            uint32_t H = (LBA / SectorsPerTrack) % HeadsPerCylinder;
            uint32_t S = (LBA % SectorsPerTrack) + 1;

            if (C > 1023)
            {
                C = 1023;
                H = 254;
                S = 63;
            }

            Data = { std::byte(H), std::byte((S & 0x3F) | ((C >> 2) & 0xC0)), std::byte(C & 0xFF) };
        }

        constexpr CHS() = default;

        std::array<std::byte, 3> Data;
    };

#pragma pack(push, 1)
    struct MBR
    {
        struct Partition
        {
            constexpr Partition(const L4::Disk::Partition& Part) :
                IsActive(),
                FirstCHS(CHS(Part.BlockAddress)),
                Type(Part.Type),
                LastCHS(CHS(Part.BlockAddress + Part.BlockCount)),
                FirstSectorLBA(Part.BlockAddress),
                SectorCount(Part.BlockCount)
            {
            }

            constexpr Partition() :
                IsActive(),
                FirstCHS(),
                Type(),
                LastCHS(),
                FirstSectorLBA(),
                SectorCount()
            {
            }

            uint8_t IsActive;
            CHS FirstCHS;
            uint8_t Type;
            CHS LastCHS;
            uint32_t FirstSectorLBA;
            uint32_t SectorCount;
        };
        static_assert(sizeof(Partition) == 16);

        constexpr MBR(std::span<const L4::Disk::Partition> Partitions) :
            Boot(),
            DiskSignature(),
            Reserved(),
            Partitions(),
            BootSignature(0xAA55)
        {
            if (Partitions.size() > this->Partitions.size())
            {
                Partitions = Partitions.first(this->Partitions.size());
            }

            std::ranges::copy(Partitions, this->Partitions.begin());
        }

        std::span<const std::byte, 512> AsBytes() const noexcept
        {
            return std::as_bytes(std::span<const MBR, 1>(this, 1));
        }

        std::array<std::byte, 440> Boot;
        uint32_t DiskSignature;
        uint16_t Reserved;
        std::array<Partition, 4> Partitions;
        uint16_t BootSignature;
    };
#pragma pack(pop)
    static_assert(sizeof(MBR) == 512);
}