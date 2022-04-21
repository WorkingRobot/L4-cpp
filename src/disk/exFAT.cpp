#include "exFAT.h"

#include <assert.h>
#include <stdint.h>
#include "../utils/Align.h"
#include "../utils/Random.h"
#include "../tree/IntervalTree.h"

#include <memory>

namespace L4
{
    static constexpr uint32_t SectorBits = 12;
    static constexpr uint32_t SectorsPerClusterBits = 9;

    static_assert(9 <= SectorBits && SectorBits <= 12, "Sector size must be between 512 and 4096 bytes");
    static_assert(1 <= SectorsPerClusterBits && SectorsPerClusterBits <= 25 - SectorBits, "Sector size must be between 1 sector and 32 MB");

    static constexpr uint32_t SectorSize = 1 << SectorBits;
    static constexpr uint32_t SectorsPerCluster = 1 << SectorsPerClusterBits;
    static constexpr uint32_t ClusterSize = SectorSize * SectorsPerCluster;

    struct Guid
    {
        uint32_t A;
        uint32_t B;
        uint32_t C;
        uint32_t D;
    };

#pragma pack(push, 1)
    struct BootSector
    {
        char JumpBoot[3];
        uint64_t FileSystemName;
        char MustBeZero[53];
        uint64_t PartitionOffset;
        uint64_t VolumeLength;
        uint32_t FatOffset;
        uint32_t FatLength;
        uint32_t ClusterHeapOffset;
        uint32_t ClusterCount;
        uint32_t FirstClusterOfRootDirectory;
        uint32_t VolumeSerialNumber;
        uint16_t FileSystemRevision;
        uint16_t VolumeFlags;
        uint8_t BytesPerSectorShift;
        uint8_t SectorsPerClusterShift;
        uint8_t NumberOfFats;
        uint8_t DriveSelect;
        uint8_t PercentInUse;
        char Reserved[7];
        char BootCode[390];
        uint16_t BootSignature;
        char ExcessSpace[SectorSize - 512];
    };
    static_assert(sizeof(BootSector) == SectorSize, "Boot sector must be 1 sector long");

    struct ExtendedBootSector
    {
        char ExtendedBootCode[SectorSize - 4];
        uint32_t ExtendedBootSignature;
    };
    static_assert(sizeof(ExtendedBootSector) == SectorSize, "Extended boot sector must be 1 sector long");

    struct NullOEMParameter
    {
        char Reserved[32];
    };
    static_assert(sizeof(NullOEMParameter) == 32, "Null OEM parameter data must be 32 bytes long");

    struct FlashOEMParameter
    {
        uint32_t EraseBlockSize;
        uint32_t PageSize;
        uint32_t SpareSectors;
        uint32_t RandomAccessTime;
        uint32_t ProgrammingTime;
        uint32_t ReadCycle;
        uint32_t WriteCycle;
        char Reserved[4];
    };
    static_assert(sizeof(FlashOEMParameter) == 32, "Flash OEM parameter data must be 32 bytes long");

    struct OEMParameter
    {
        Guid ParameterGuid;
        union
        {
            char CustomDefined[32];
            NullOEMParameter Null;
            FlashOEMParameter Flash;
        };
    };
    static_assert(sizeof(OEMParameter) == 48, "OEM parameter must be 48 bytes long");

    struct OEMParameterSector
    {
        OEMParameter Parameters[10];
        char Reserved[SectorSize - 480];
    };
    static_assert(sizeof(OEMParameterSector) == SectorSize, "OEM parameter boot sector must be 1 sector long");

    struct BootRegion
    {
        BootSector BootSector;
        ExtendedBootSector ExtendedBootSectors[8];
        OEMParameterSector OEMParameters;
        char Reserved[SectorSize];
        uint32_t BootChecksum[SectorSize / sizeof(uint32_t)];
    };
    static_assert(sizeof(BootRegion) == 12 * SectorSize, "Boot region must be 12 sectors long");

    struct PrimaryDirectoryEntry
    {
        uint8_t SecondaryCount;
        uint16_t SetChecksum;
        uint16_t GeneralPrimaryFlags;
        union
        {
            char CustomDefined[14];
        };
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(PrimaryDirectoryEntry) == 31, "Primary directory entry must be 31 bytes long");

    struct SecondaryDirectoryEntry
    {
        uint8_t GeneralSecondaryFlags;
        union
        {
            char CustomDefined[18];
        };
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(SecondaryDirectoryEntry) == 31, "Secondary directory entry must be 31 bytes long");

    struct AllocationBitmapDirectoryEntry
    {
        uint8_t BitmapFlags;
        char Reserved[18];
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(AllocationBitmapDirectoryEntry) == 31, "Allocation bitmap directory entry must be 31 bytes long");

    struct UpcaseTableDirectoryEntry
    {
        char Reserved1[3];
        uint32_t TableChecksum;
        char Reserved2[12];
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(UpcaseTableDirectoryEntry) == 31, "Up-case table directory entry must be 31 bytes long");

    struct VolumeLabelDirectoryEntry
    {
        uint8_t CharacterCount;
        wchar_t VolumeLabel[11];
        char Reserved[8];
    };
    static_assert(sizeof(VolumeLabelDirectoryEntry) == 31, "Volume label directory entry must be 31 bytes long");

    struct FileDirectoryEntry
    {
        uint8_t SecondaryCount;
        uint16_t SetChecksum;
        uint16_t FileAttributes;
        char Reserved1[2];
        uint32_t CreateTimestamp;
        uint32_t LastModifiedTimestamp;
        uint32_t LastAccessedTimestamp;
        uint8_t Create10msIncrement;
        uint8_t LastModified10msIncrement;
        uint8_t CreateUtcOffset;
        uint8_t LastModifiedUtcOffset;
        uint8_t LastAccessedUtcOffset;
        char Reserved2[7];
    };
    static_assert(sizeof(FileDirectoryEntry) == 31, "File directory entry must be 31 bytes long");

    struct VolumeGuidDirectoryEntry
    {
        uint8_t SecondaryCount;
        uint16_t SetChecksum;
        uint16_t GeneralPrimaryFlags;
        Guid VolumeGuid;
        char Reserved[10];
    };
    static_assert(sizeof(VolumeGuidDirectoryEntry) == 31, "Volume GUID directory entry must be 31 bytes long");

    struct TexFatPaddingDirectoryEntry
    {
        char Reserved[31]; // No idea
    };
    static_assert(sizeof(TexFatPaddingDirectoryEntry) == 31, "TexFAT padding directory entry must be 31 bytes long");

    struct StreamExtensionDirectoryEntry
    {
        uint8_t GeneralSecondaryFlags;
        char Reserved1[1];
        uint8_t NameLength;
        uint16_t NameHash;
        char Reserved[2];
        uint64_t ValidDataLength;
        char Reserved3[4];
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(StreamExtensionDirectoryEntry) == 31, "Stream extension directory entry must be 31 bytes long");

    struct FileNameDirectoryEntry
    {
        uint8_t GeneralSecondaryFlags;
        wchar_t FileName[15];
    };
    static_assert(sizeof(FileNameDirectoryEntry) == 31, "File name directory entry must be 31 bytes long");

    struct VendorExtensionDirectoryEntry
    {
        uint8_t GeneralSecondaryFlags;
        Guid VendorGuid;
        char VendorDefined[14];
    };
    static_assert(sizeof(VendorExtensionDirectoryEntry) == 31, "Vendor extension directory entry must be 31 bytes long");

    struct VendorAllocationDirectoryEntry
    {
        uint8_t GeneralSecondaryFlags;
        Guid VendorGuid;
        char VendorDefined[2];
        uint32_t FirstCluster;
        uint64_t DataLength;
    };
    static_assert(sizeof(VendorAllocationDirectoryEntry) == 31, "Vendor allocation directory entry must be 31 bytes long");

    struct EndOfDirectoryEntry
    {
        char Reserved[31];
    };
    static_assert(sizeof(EndOfDirectoryEntry) == 31, "End of directory entry must be 31 bytes long");

    template<class T>
    struct DirectoryEntryTraits
    {
    };

    template<>
    struct DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCategory = 0;
    };

    template<>
    struct DirectoryEntryTraits<SecondaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCategory = 1;
    };

    template<>
    struct DirectoryEntryTraits<AllocationBitmapDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 1;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<UpcaseTableDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 2;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<VolumeLabelDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 3;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<FileDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 5;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<VolumeGuidDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 0;
        static constexpr uint8_t TypeImportance = 1;
    };

    template<>
    struct DirectoryEntryTraits<TexFatPaddingDirectoryEntry> : public DirectoryEntryTraits<PrimaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 1;
        static constexpr uint8_t TypeImportance = 1;
    };

    template<>
    struct DirectoryEntryTraits<StreamExtensionDirectoryEntry> : public DirectoryEntryTraits<SecondaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 0;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<FileNameDirectoryEntry> : public DirectoryEntryTraits<SecondaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 1;
        static constexpr uint8_t TypeImportance = 0;
    };

    template<>
    struct DirectoryEntryTraits<VendorExtensionDirectoryEntry> : public DirectoryEntryTraits<SecondaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 0;
        static constexpr uint8_t TypeImportance = 1;
    };

    template<>
    struct DirectoryEntryTraits<VendorAllocationDirectoryEntry> : public DirectoryEntryTraits<SecondaryDirectoryEntry>
    {
        static constexpr uint8_t TypeCode = 1;
        static constexpr uint8_t TypeImportance = 1;
    };

    struct DirectoryEntry
    {
        uint8_t EntryType;
        union
        {
            EndOfDirectoryEntry EndOfDirectory;
            PrimaryDirectoryEntry Primary;
            SecondaryDirectoryEntry Secondary;
            AllocationBitmapDirectoryEntry AllocationBitmap;
            UpcaseTableDirectoryEntry UpcaseTable;
            VolumeLabelDirectoryEntry VolumeLabel;
            FileDirectoryEntry File;
            VolumeGuidDirectoryEntry VolumeGuid;
            TexFatPaddingDirectoryEntry TexFatPadding;
            StreamExtensionDirectoryEntry StreamExtension;
            FileNameDirectoryEntry FileName;
            VendorExtensionDirectoryEntry VendorExtension;
            VendorAllocationDirectoryEntry VendorAllocation;
        };
    };
    static_assert(sizeof(DirectoryEntry) == 32, "Directory entry must be 32 bytes long");

#pragma pack(pop)

    template <size_t Size>
    constexpr uint32_t Checksum(const uint8_t(&Data)[Size]) noexcept
    {
        uint32_t Checksum = 0;
        for (size_t i = 0; i < Size; ++i)
        {
            Checksum = ((Checksum << 31) | (Checksum >> 1)) + Data[i];
        }
        return Checksum;
    }

    template <size_t Size>
    constexpr uint16_t NameChecksum(const wchar_t(&Data)[Size]) noexcept
    {
        uint16_t Checksum = 0;
        for (size_t i = 0; i < Size * sizeof(wchar_t); ++i)
        {
            Checksum = ((Checksum << 15) | (Checksum >> 1)) + ((uint8_t*)Data)[i];
        }
        return Checksum;
    }

    constexpr uint16_t SetChecksum(const DirectoryEntry* Entry) noexcept
    {
        size_t Size = (1llu + Entry->Primary.SecondaryCount) * sizeof(DirectoryEntry);
        uint16_t Checksum = 0;
        for (size_t i = 0; i < Size; ++i)
        {
            if (i == 2 || i == 3)
            {
                continue;
            }
            Checksum = ((Checksum << 15) | (Checksum >> 1)) + ((uint8_t*)Entry)[i];
        }
        return Checksum;
    }

    template<class T, bool SkipFields = std::is_same_v<T, BootSector>>
    uint32_t BootChecksum(const T& Value, uint32_t Checksum = 0)
    {
        auto ValuePtr = (const uint8_t*)&Value;

        for (size_t i = 0; i < sizeof(T); ++i)
        {
            if constexpr (SkipFields)
            {
                if (i == 106 || i == 107 || i == 112)
                {
                    continue;
                }
            }
            Checksum = ((Checksum << 31) | (Checksum >> 1)) + ValuePtr[i];
        }

        return Checksum;
    }

    uint32_t BootChecksumRegion(const BootRegion& BootRegion)
    {
        uint32_t Checksum = BootChecksum(BootRegion.BootSector);
        Checksum = BootChecksum(BootRegion.ExtendedBootSectors, Checksum);
        Checksum = BootChecksum(BootRegion.OEMParameters, Checksum);
        Checksum = BootChecksum(BootRegion.Reserved, Checksum);
        return Checksum;
    }

    template<class T>
    static void AssertRange_(T Variable, T Minimum, T Maximum)
    {
        assert(Minimum <= Variable && Variable <= Maximum);
    }

    void Validate(const BootSector& BootSector)
    {
        return;
//#define AssertRange(Variable, Minimum, Maximum) assert((Minimum) <= (Variable) && (Variable) <= (Maximum))
#define AssertRange(Variable, Minimum, Maximum) AssertRange_((Variable), (decltype(Variable))(Minimum), (decltype(Variable))(Maximum))
#define AssertEquals(Variable, Value) assert((Variable) == (Value))

        AssertRange(BootSector.VolumeLength, 1llu << (20llu - BootSector.BytesPerSectorShift), ~0llu - 1);

        AssertRange(BootSector.FatOffset, 24u, BootSector.ClusterHeapOffset - (BootSector.FatLength * BootSector.NumberOfFats));

        AssertRange(BootSector.FatLength, Align((BootSector.ClusterCount + 2) * sizeof(uint32_t), 1 << BootSector.BytesPerSectorShift) / 1 << BootSector.BytesPerSectorShift, (BootSector.ClusterHeapOffset - BootSector.FatOffset) / BootSector.NumberOfFats);

        AssertRange(BootSector.ClusterHeapOffset, BootSector.FatOffset + BootSector.FatLength * BootSector.NumberOfFats, std::min((1llu << 32) - 1, BootSector.VolumeLength - (BootSector.ClusterCount * (1 << BootSector.SectorsPerClusterShift))));
        
        AssertEquals(BootSector.ClusterCount, std::min((BootSector.VolumeLength - BootSector.ClusterHeapOffset) / (1 << BootSector.SectorsPerClusterShift), (1llu << 32) - 11));

        AssertRange(BootSector.FirstClusterOfRootDirectory, 2, BootSector.ClusterCount + 1);

        AssertRange(BootSector.BytesPerSectorShift, 9, 12);

        AssertRange(BootSector.SectorsPerClusterShift, 0, 25 - BootSector.BytesPerSectorShift);

        AssertRange(BootSector.NumberOfFats, 1, 2);

        AssertRange(BootSector.PercentInUse, 0, 100); // or 0xFF

#undef AssertEquals
#undef AssertRange
    }

    extern const uint8_t ExFatUpcaseTable[5836];
    class ExFatSystem
    {
    public:
        ExFatSystem(uint64_t PartitionSectorOffset, uint64_t PartitionSectorCount) :
            BootRegion{
                .BootSector{
                    .JumpBoot{ (char)0xEB, 0x76, (char)0x90 },
                    .FileSystemName = 0x2020205441465845,
                    .MustBeZero = {},
                    .FileSystemRevision = 0x0100,
                    .VolumeFlags = 0x0000,
                    .DriveSelect = 0x80,
                    .Reserved{},
                    .BootCode{},
                    .BootSignature = 0xAA55,
                    .ExcessSpace{}
                },
                .ExtendedBootSectors{},
                .OEMParameters{},
                .Reserved{}
            }
        {
            {
                // Can also be 0 (this means Windows is supposed to ignore this field)
                BootRegion.BootSector.PartitionOffset = PartitionSectorOffset;

                BootRegion.BootSector.VolumeLength = PartitionSectorCount;

                BootRegion.BootSector.FatOffset = 0x80;

                // Generous maximum estimate
                BootRegion.BootSector.ClusterCount = BootRegion.BootSector.VolumeLength / SectorsPerCluster;
                BootRegion.BootSector.FatLength = Align((BootRegion.BootSector.ClusterCount + 2llu) * sizeof(uint32_t), SectorSize) / SectorSize;

                BootRegion.BootSector.ClusterHeapOffset = BootRegion.BootSector.FatOffset + BootRegion.BootSector.FatLength;

                BootRegion.BootSector.ClusterCount = std::min((BootRegion.BootSector.VolumeLength - BootRegion.BootSector.ClusterHeapOffset) / SectorsPerCluster, (1llu << 32) - 11);

                // BootRegion.BootSector.FirstClusterOfRootDirectory = 0;

                BootRegion.BootSector.VolumeSerialNumber = Random<uint32_t>(); // 0xABB5A168

                BootRegion.BootSector.BytesPerSectorShift = SectorBits;

                BootRegion.BootSector.SectorsPerClusterShift = SectorsPerClusterBits;

                BootRegion.BootSector.NumberOfFats = 1;

                BootRegion.BootSector.PercentInUse = 0;

                for (auto& ExtendedBootSector : BootRegion.ExtendedBootSectors)
                {
                    ExtendedBootSector.ExtendedBootSignature = 0xAA550000;
                }

                Intervals.Add(0, 12, &BootRegion);
                Intervals.Add(12, 12, &BootRegion);
            }

            {
                FatSize = 2llu + BootRegion.BootSector.ClusterCount;
                Fat = std::make_unique<uint32_t[]>(FatSize);
                Fat[0] = 0xFFFFFFF8;
                Fat[1] = 0xFFFFFFFF;

                Intervals.Add(BootRegion.BootSector.FatOffset, BootRegion.BootSector.FatLength, Fat.get(), FatSize);
            }

            uint32_t AllocationBitmapCluster;
            {
                AllocationBitmapSize = Align<8>(BootRegion.BootSector.ClusterCount + 2llu) / 8;
                AllocationBitmap = std::make_unique<uint8_t[]>(AllocationBitmapSize);

                AllocationBitmapCluster = AllocateClusters(AllocationBitmap.get(), AllocationBitmapSize);
            }

            uint32_t UpcaseTableCluster = AllocateClusters(ExFatUpcaseTable, sizeof(ExFatUpcaseTable));

            BootRegion.BootSector.FirstClusterOfRootDirectory = AllocateClusters(RootDirectory, sizeof(RootDirectory));

            Validate(BootRegion.BootSector);

            std::fill(std::begin(BootRegion.BootChecksum), std::end(BootRegion.BootChecksum), BootChecksumRegion(BootRegion));

            RootDirectory[0] = CreateDirectoryEntry(VolumeLabelDirectoryEntry{}, false);
            RootDirectory[1] = CreateDirectoryEntry(AllocationBitmapDirectoryEntry{
                .BitmapFlags = 0,
                .FirstCluster = AllocationBitmapCluster,
                .DataLength = AllocationBitmapSize
            });
            RootDirectory[2] = CreateDirectoryEntry(UpcaseTableDirectoryEntry{
                .TableChecksum = Checksum(ExFatUpcaseTable),
                .FirstCluster = UpcaseTableCluster,
                .DataLength = sizeof(ExFatUpcaseTable)
            });

            AmogusDirData = std::make_unique<char[]>(ClusterSize);
            RootDirectory[3] = CreateDirectoryEntry(FileDirectoryEntry{
                .SecondaryCount = 2,
                .FileAttributes = 1 << 4,
                .CreateTimestamp = 0x545712A9,
                .LastModifiedTimestamp = 0x545712A9,
                .LastAccessedTimestamp = 0x545712A9,
                .Create10msIncrement = 0x54,
                .LastModified10msIncrement = 0x54,
                .CreateUtcOffset = 0xE0,
                .LastModifiedUtcOffset = 0xE0,
                .LastAccessedUtcOffset = 0xE0
            });
            RootDirectory[4] = CreateDirectoryEntry(StreamExtensionDirectoryEntry{
                .GeneralSecondaryFlags = 0x03,
                .NameLength = 6,
                .NameHash = NameChecksum(L"amogus"),
                .ValidDataLength = ClusterSize,
                .FirstCluster = AllocateClusters(AmogusDirData.get(), ClusterSize),
                .DataLength = ClusterSize
            });
            RootDirectory[5] = CreateDirectoryEntry(FileNameDirectoryEntry{
                .FileName = L"amogus"
            });
            RootDirectory[3].Primary.SetChecksum = SetChecksum(&RootDirectory[3]);
        }

        void AllocateBitmapCluster(uint32_t Cluster)
        {
            AllocationBitmap[Cluster / 8] |= 1 << (Cluster % 8);
        }

        uint32_t AllocateClusters(uint64_t DataLength)
        {
            auto Ret = NextCluster;
            auto ClusterCount = Align<ClusterSize>(DataLength) / ClusterSize;
            for (; ClusterCount > 1; --ClusterCount, NextCluster++)
            {
                AllocationBitmap[NextCluster / 8] |= 1 << (NextCluster % 8);
                Fat[NextCluster] = NextCluster + 1;
            }
            AllocationBitmap[NextCluster / 8] |= 1 << (NextCluster % 8);
            Fat[NextCluster] = 0xFFFFFFFF;
            NextCluster++;
            return Ret;
        }

        uint32_t AllocateClusters(const void* Data, uint64_t DataLength)
        {
            auto FirstCluster = AllocateClusters(DataLength);
            auto FirstClusterOffset = BootRegion.BootSector.ClusterHeapOffset + (FirstCluster - BeginCluster) * SectorsPerCluster;
            Intervals.Add(FirstClusterOffset, Align<ClusterSize>(DataLength) / SectorSize, Data, DataLength);
            return FirstCluster;
        }

        template<class T>
        DirectoryEntry CreateDirectoryEntry(T&& Val, bool InUse = true)
        {
            static_assert(sizeof(T) == 31);
            using Traits = DirectoryEntryTraits<T>;
            union
            {
                T Entry;
                EndOfDirectoryEntry Data;
            };
            Entry = std::move(Val);
            return DirectoryEntry{
                .EntryType = uint8_t(Traits::TypeCode | Traits::TypeImportance << 5 | Traits::TypeCategory << 6 | InUse << 7),
                .EndOfDirectory = Data
            };
        }

        const IntervalList& GetIntervalList() const
        {
            return Intervals;
        }

    private:
        static constexpr uint32_t BeginCluster = 2;
        uint32_t NextCluster = BeginCluster;

        BootRegion BootRegion;

        std::unique_ptr<uint32_t[]> Fat;
        size_t FatSize;

        std::unique_ptr<uint8_t[]> AllocationBitmap;
        size_t AllocationBitmapSize;

        DirectoryEntry RootDirectory[6];

        std::unique_ptr<char[]> AmogusDirData;

        IntervalList Intervals;
    };

    ExFatSystemPublic::ExFatSystemPublic(uint64_t PartitionSectorOffset, uint64_t PartitionSectorCount) :
        Private(std::make_unique<ExFatSystemPrivate>(PartitionSectorOffset, PartitionSectorCount))
    {

    }

    ExFatSystemPublic::~ExFatSystemPublic()
    {

    }

    const IntervalList& ExFatSystemPublic::GetIntervalList() const
    {
        return Private->GetIntervalList();
    }
}