#pragma once

#include "Stream.h"

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    class Archive
    {
    public:
        Archive(libL4::Handle ClientHandle);

        libL4::Marshal::ArchiveIdentity GetIdentity() const;

        void SetIdentity(const libL4::Marshal::ArchiveIdentity& NewIdentity);

        uint32_t GetStreamCount() const;

        uint32_t GetSectorSize() const;

        uint32_t GetStreamIdxFromId(const std::u8string& Id) const;

        const Stream OpenStreamRead(uint32_t StreamIdx) const;

        Stream OpenStreamWrite(uint32_t StreamIdx);

        const Stream OpenStreamRead(const std::u8string& Id) const;

        Stream OpenStreamWrite(const std::u8string& Id);

    private:
        libL4::Handle ClientHandle;
    };
}