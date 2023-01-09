#pragma once

#include "archive/Archive.h"
#include "archive/ArchiveWritable.h"
#include "Stream.h"

#include <deque>
#include <memory>

namespace L4::Manager
{
    class Archive
    {
    public:
        Archive(const std::filesystem::path& Path, std::false_type IsWritable);

        Archive(const std::filesystem::path& Path, std::true_type IsWritable);

        libL4::Marshal::ArchiveIdentity GetIdentity() const;

        void SetIdentity(const libL4::Marshal::ArchiveIdentity& NewIdentity);

        uint32_t GetStreamCount() const;

        uint32_t GetSectorSize() const;

        Stream OpenStream(std::u8string_view Id);

    private:
        friend class ClientInterface;
        libL4::Handle OpenStreamInternal(std::u8string_view Id);
        bool CloseStreamInternal(libL4::Handle Handle);

        std::forward_list<Stream> Streams;

        std::variant<L4::Archive::Archive, L4::Archive::ArchiveWritable> Impl;
    };
}