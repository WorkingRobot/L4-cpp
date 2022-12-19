#pragma once

#include "../PatchChunk.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    enum class SQPKType : uint8_t
    {
        Add = 'A',
        Delete = 'D',
        Expand = 'E',
        File = 'F',
        Header = 'H',
        Index = 'I',
        Patch = 'X',
        Target = 'T',
    };

    struct SQPK : public PatchChunk
    {
    protected:
        SQPK() = default;

    public:
        static std::unique_ptr<PatchChunk> Read(Stream& Stream);
    };
}