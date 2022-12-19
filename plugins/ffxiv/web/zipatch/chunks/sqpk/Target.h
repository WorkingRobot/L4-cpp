#pragma once

#include "../../PlatformId.h"
#include "../../RegionId.h"
#include "../../SqpkFile.h"
#include "../SQPK.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKTarget : public SQPK
    {
        PlatformId Platform;
        RegionId Region;
        bool IsDebug;
        uint16_t Version;

        void Deserialize(Stream& Stream)
        {
            Stream.Skip(3); // Alignment

            Platform = Stream.Read<BE<PlatformId>>();
            Region = Stream.Read<BE<RegionId>>();
            IsDebug = Stream.Read<BE<uint16_t>>() != 0;
            Version = Stream.Read<BE<uint16_t>>();

            // Rest of the data is weird and wacky and unused
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK Target\n");
            printf("Version: %u\n", Version);
            printf("Debug: %s\n", IsDebug ? "true" : "false");
            printf("Platform: %x\n", Platform);
            printf("Region: %x\n", Region);
            printf("\n");
        }
    };
}