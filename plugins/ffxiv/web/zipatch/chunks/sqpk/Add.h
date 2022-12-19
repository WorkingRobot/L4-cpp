#pragma once

#include "../../SqpkFile.h"
#include "../SQPK.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct SQPKAdd : public SQPK
    {
        SqpkFile File;
        uint32_t BlockOffset;
        uint32_t BlockCount;
        uint32_t BlockDeleteCount;
        std::unique_ptr<std::byte[]> Data;

        void Deserialize(Stream& Stream)
        {
            Stream.Skip(3); // Alignment

            Stream >> File;

            BlockOffset = Stream.Read<BE<uint32_t>>();
            BlockCount = Stream.Read<BE<uint32_t>>();
            BlockDeleteCount = Stream.Read<BE<uint32_t>>();

            Data = std::make_unique<std::byte[]>(size_t(BlockCount) << 7);
            Stream.ReadBytes(std::span(Data.get(), size_t(BlockCount) << 7));
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK Add\n");
            printf("File: %s\n", File.GetDatFilePath().c_str());
            printf("Offset: %u\n", BlockOffset);
            printf("Size: %u\n", BlockCount);
            printf("Delete Size: %u\n", BlockDeleteCount);
            printf("\n");
        }
    };
}