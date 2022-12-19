#include "PatchFile.h"

#include "chunks/EOF_.h"

namespace L4::Plugin::FFXIV::ZiPatch
{
    static constexpr std::array<uint32_t, 3> ExpectedMagic = { 0x50495A91, 0x48435441, 0x0A1A0A0D };

    PatchFile::PatchFile(L4::Stream& Stream) :
        Stream(Stream),
        IsEOF(false)
    {
        auto Magic = Stream.Read<std::array<uint32_t, 3>>();
        if (Magic != ExpectedMagic)
        {
            throw std::invalid_argument("Invalid magic");
        }
    }

    std::unique_ptr<PatchChunk> PatchFile::ReadChunk()
    {
        auto Chunk = PatchChunk::Read(Stream);

        if (dynamic_cast<Chunks::EOF_*>(Chunk.get()))
        {
            IsEOF = true;
        }

        return Chunk;
    }

    bool PatchFile::AtEOF() const noexcept
    {
        return IsEOF;
    }

}