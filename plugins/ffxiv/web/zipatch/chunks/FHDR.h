#pragma once

#include "../PatchChunk.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    enum class FHDRVersion : uint32_t
    {
        Version2 = 0x200,
        Version3 = 0x300,
    };

    enum class FHDRPatchType : uint32_t
    {
        DIFF = 0x46464944,
        HIST = 0x54534948,
    };

    struct FHDR : public PatchChunk
    {
        FHDRVersion Version;

        FHDRPatchType PatchType;
        uint32_t EntryFileCount;

        uint32_t ADIRCount;
        uint32_t DELDCount;
        uint64_t DeleteDataSize;
        uint32_t MinorVersion;
        uint32_t RepositoryName;
        uint32_t Commands;

        uint32_t SQPKACount;
        uint32_t SQPKDCount;
        uint32_t SQPKECount;
        uint32_t SQPKHCount;
        uint32_t SQPKFCount;

        void Deserialize(Stream& Stream)
        {
            Version = Stream.Read<BE<FHDRVersion>>();
            PatchType = Stream.Read<FHDRPatchType>();
            EntryFileCount = Stream.Read<BE<uint32_t>>();

            if (Version >= FHDRVersion::Version3)
            {
                ADIRCount = Stream.Read<BE<uint32_t>>();
                DELDCount = Stream.Read<BE<uint32_t>>();
                DeleteDataSize = Stream.Read<BE<uint32_t>>() | ((uint64_t)Stream.Read<BE<uint32_t>>() << 32);
                MinorVersion = Stream.Read<BE<uint32_t>>();
                RepositoryName = Stream.Read<BE<uint32_t>>();
                Commands = Stream.Read<BE<uint32_t>>();
                SQPKACount = Stream.Read<BE<uint32_t>>();
                SQPKDCount = Stream.Read<BE<uint32_t>>();
                SQPKECount = Stream.Read<BE<uint32_t>>();
                SQPKHCount = Stream.Read<BE<uint32_t>>();
                SQPKFCount = Stream.Read<BE<uint32_t>>();
            }

            Stream.Skip(sizeof(uint64_t)); // All 00s
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("File Header\n");
            printf("Version: %x\n", Version);
            printf("Patch Type: %.*s\n", 4, &PatchType);
            printf("File Count: %u\n", EntryFileCount);
            printf("\n");
        }
    };
}