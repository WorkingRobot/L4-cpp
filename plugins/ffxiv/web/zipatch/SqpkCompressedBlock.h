#pragma once

#include "Endian.h"

#include <streams/Stream.h>
#include <utils/Align.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    struct SqpkCompressedBlock
    {
        uint32_t DataSizeCompressed;
        uint32_t DataSizeUncompressed;
        std::unique_ptr<std::byte[]> Data;

        void Deserialize(Stream& Stream)
        {
            auto BlockBegin = Stream.Tell();

            auto HeaderSize = Stream.Read<uint32_t>();
            Stream.Skip(sizeof(uint32_t)); // Padding

            DataSizeCompressed = Stream.Read<uint32_t>();
            DataSizeUncompressed = Stream.Read<uint32_t>();

            // Is uncompressed
            if (DataSizeCompressed == 32000)
            {
                Data = std::make_unique<std::byte[]>(DataSizeUncompressed);
                Stream.ReadBytes(std::span(Data.get(), DataSizeUncompressed));
            }
            else
            {
                Data = std::make_unique<std::byte[]>(DataSizeCompressed);
                Stream.ReadBytes(std::span(Data.get(), DataSizeCompressed));
            }

            auto BlockEnd = Stream.Tell();
            auto BlockSize = BlockEnd - BlockBegin;

            // https://xiv.dev/data-files/zipatch/sqpk#type-f-file-operation
            // https://github.com/goatcorp/FFXIVQuickLauncher/blob/77115d3f7d920179ce83f0708ec97ba2450ce795/src/XIVLauncher.Common/Patching/ZiPatch/Util/SqpkCompressedBlock.cs#L13
            // The block is actually aligned to 128 bytes. It's just aligned to the 
            // beginning of the block, not to the beginning of the data or the patch file.
            auto AlignedBlockSize = Align<128>(BlockSize);
            Stream.Skip(AlignedBlockSize - BlockSize);
        }

        uint32_t GetSize() const noexcept
        {
            return DataSizeCompressed == 32000 ? DataSizeUncompressed : DataSizeCompressed;
        }

        uint32_t GetSizeUncompressed() const noexcept
        {
            return DataSizeUncompressed;
        }
    };
}