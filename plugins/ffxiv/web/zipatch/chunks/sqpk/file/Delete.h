#pragma once

#include "../File.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKFileDelete : public SQPKFile
    {
        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK File Delete\n");
            printf("File: %s\n", this->FileName.c_str());
            printf("\n");
        }
    };
}