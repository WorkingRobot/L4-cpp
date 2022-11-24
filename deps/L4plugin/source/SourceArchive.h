#pragma once

#include "archive/Archive.h"
#include "SourceStream.h"

namespace L4
{
    class SourceArchive
    {
    public:
        SourceArchive(ArchiveBase<true>&& Archive);

        [[nodiscard]] Source::AppIdentity GetIdentity() const;

        void SetIdentity(const Source::AppIdentity& NewIdentity);

        [[nodiscard]] uint32_t GetStreamCount() const;

        [[nodiscard]] uint32_t GetSectorSize() const;

        [[nodiscard]] uint32_t GetStreamIdxFromId(std::u8string_view Id) const;

        SourceStream* OpenStreamRead(uint32_t StreamIdx);

        SourceStream* OpenStreamWrite(uint32_t StreamIdx);

        void CloseStream(const SourceStream& Stream);

    private:
        ArchiveBase<true> Archive;
        std::vector<std::unique_ptr<SourceStream>> Streams;
    };
}