#pragma once

#include "../File.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKFileRemove : public SQPKFile
    {
        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK File Remove\n");
            printf("File: %s\n", this->FileName.c_str());
            printf("Expansion: %04x\n", this->ExpansionId);
            printf("\n");
        }
    };
}