#pragma once

#include "../../SqpkFile.h"
#include "../SQPK.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    enum class SQPKHeaderFileType : uint8_t
    {
        Dat = 'D',
        Index = 'I'
    };

    enum class SQPKHeaderHeaderType : uint8_t
    {
        Version = 'V',
        Index = 'I',
        Data = 'D'
    };

    struct SQPKHeader : public SQPK
    {
        SQPKHeaderHeaderType HeaderType;
        SQPKHeaderFileType FileType;
        SqpkFile File;
        std::array<std::byte, 1024> Data;

        void Deserialize(Stream& Stream)
        {
            FileType = Stream.Read<BE<SQPKHeaderFileType>>();
            HeaderType = Stream.Read<BE<SQPKHeaderHeaderType>>();

            Stream.Skip(1); // Alignment

            Stream >> File;

            Stream >> Data;
        }

        void Apply(PatchCtx& Ctx) override
        {
            printf("SQPK Header\n");
            printf("File: %s\n", (FileType == SQPKHeaderFileType::Dat ? File.GetDatFilePath() : File.GetIndexFilePath()).c_str());
            printf("Header: %c\n", HeaderType);
            printf("\n");
        }
    };
}