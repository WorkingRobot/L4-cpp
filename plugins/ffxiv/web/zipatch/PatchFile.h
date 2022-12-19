#pragma once

#include "PatchChunk.h"

#include <streams/Stream.h>

#include <string>

namespace L4::Plugin::FFXIV::ZiPatch
{
    class PatchFile
    {
    public:
        PatchFile(L4::Stream& Stream);

        std::unique_ptr<PatchChunk> ReadChunk();

        bool AtEOF() const noexcept;

    private:
        L4::Stream& Stream;
        bool IsEOF;
    };
}