#pragma once

#include "../PatchChunk.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct EOF_ : public PatchChunk
    {
        void Deserialize(Stream& Stream)
        {
            // 32 bytes of 00 for padding
            Stream.Skip(32);
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("End of Patch\n");
            printf("\n");
        }
    };
}