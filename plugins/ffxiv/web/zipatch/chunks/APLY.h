#pragma once

#include "../PatchChunk.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    enum class APLYOption : uint32_t
    {
        IgnoreMissing = 1,
        IgnoreOldMismatch = 2,
    };

    struct APLY : public PatchChunk
    {
        APLYOption Option;
        bool Value;

        void Deserialize(Stream& Stream)
        {
            Option = Stream.Read<BE<APLYOption>>();

            auto OptionSize = Stream.Read<BE<uint32_t>>(); // Always 4

            Value = Stream.Read<BE<uint32_t>>();
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("Apply Option\n");
            printf("%u: %s\n", Option, Value ? "true" : "false");
            printf("\n");
        }
    };
}