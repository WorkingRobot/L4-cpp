#pragma once

#include "../../../SqpkCompressedBlock.h"
#include "../File.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKFileAdd : public SQPKFile
    {
        std::vector<SqpkCompressedBlock> CompressedBlocks;

        void Deserialize(Stream& Stream)
        {
            SQPKFile::Deserialize(Stream);

            while (Stream.Tell() != Stream.Size())
            {
                Stream >> CompressedBlocks.emplace_back();
            }
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK File Add\n");
            printf("File: %s\n", this->FileName.c_str());
            printf("Expansion: %04x\n", this->ExpansionId);
            printf("Offset: %lld\n", this->FileOffset);
            printf("Size: %lld\n", this->FileSize);
            printf("%llu Blocks\n", this->CompressedBlocks.size());
            printf("\n");
        }
    };
}