#pragma once

#include "../Endian.h"
#include "../PatchChunk.h"

namespace L4::Plugin::FFXIV::ZiPatch::Chunks
{
    struct ADIR : public PatchChunk
    {
        std::string DirectoryName;

        void Deserialize(Stream& Stream)
        {
            DirectoryName.resize_and_overwrite(Stream.Read<BE<uint32_t>>(), [&Stream](char* Buffer, size_t Count) {
                Stream.ReadBytes(std::as_writable_bytes(std::span(Buffer, Count)));
                return Count;
            });
        }
    };
}