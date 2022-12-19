#pragma once

#include <streams/Stream.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    enum class PatchChunkType : uint32_t
    {
        FHDR = 0x52444846,
        APLY = 0x594C5041,
        ADIR = 0x52494441,
        DELD = 0x444C4544,
        SQPK = 0x4B505153,
        EOF_ = 0x5F464F45,
        APFS = 0x53465041,
        XXXX = 0x58585858,
    };

    struct PatchCtx
    {
    
    };

    struct PatchChunk
    {
    protected:
        PatchChunk() = default;

    public:
        virtual ~PatchChunk() = default;

        virtual void Apply(PatchCtx& Ctx)
        {
            printf("unk\n");
        }

        static std::unique_ptr<PatchChunk> Read(Stream& Stream);
    };
}