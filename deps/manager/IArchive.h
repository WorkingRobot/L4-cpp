#pragma once

#include "IStream.h"

#include <memory>
#include <vector>

namespace L4::Manager
{
    class IArchive
    {
    public:
        IArchive() = default;

        virtual ~IArchive() = default;

        virtual libL4::Marshal::ArchiveIdentity GetIdentity() const = 0;

        virtual void SetIdentity(const libL4::Marshal::ArchiveIdentity& NewIdentity) = 0;

        virtual uint32_t GetStreamCount() const = 0;

        virtual uint32_t GetSectorSize() const = 0;

        virtual uint32_t GetStreamIdxFromId(const std::u8string& Id) const = 0;

        virtual std::unique_ptr<IStream> OpenStreamRead(uint32_t StreamIdx) const = 0;

        virtual std::unique_ptr<IStream> OpenStreamWrite(uint32_t StreamIdx) = 0;

    private:
        friend class ClientInterface;
        libL4::Handle OpenStreamInternal(uint32_t StreamIdx, bool IsWrite);
        bool CloseStreamInternal(libL4::Handle Stream);

        std::vector<std::unique_ptr<IStream>> Streams;
    };
}