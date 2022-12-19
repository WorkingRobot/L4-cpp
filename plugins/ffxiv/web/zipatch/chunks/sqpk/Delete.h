#pragma once

#include "../../SqpkFile.h"
#include "../SQPK.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKDelete : public SQPK
    {
        SqpkFile File;
        uint32_t BlockOffset;
        uint32_t BlockCount;

        void Deserialize(Stream& Stream)
        {
            Stream.Skip(3); // Alignment

            Stream >> File;

            BlockOffset = Stream.Read<BE<uint32_t>>();
            BlockCount = Stream.Read<BE<uint32_t>>();
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK Delete\n");
            printf("File: %s\n", File.GetDatFilePath().c_str());
            printf("Offset: %u\n", BlockOffset);
            printf("Size: %u\n", BlockCount);
            printf("\n");
        }
    };
}