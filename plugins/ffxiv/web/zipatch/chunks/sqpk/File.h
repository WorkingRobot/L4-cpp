#pragma once

#include "../SQPK.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKFile : public SQPK
    {
        int64_t FileOffset;
        int64_t FileSize;
        uint16_t ExpansionId;
        std::string FileName;

    protected:
        SQPKFile() = default;

    public:
        static std::unique_ptr<PatchChunk> Read(Stream& Stream);
        
        void Deserialize(Stream& Stream);
    };
}