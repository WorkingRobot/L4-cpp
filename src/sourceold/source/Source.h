#pragma once

#include "Base.h"
#include "Archive.h"
#include "Upstream.h"

namespace L4::Interface
{
    enum class SourceVersion : uint32_t
    {
        Unknown,
        Initial,

        LatestPlusOne,
        Latest = LatestPlusOne - 1
    };

    class L4_CLASS_API ISource
    {
    public:
        static const SourceVersion Version = SourceVersion::Initial;

        virtual Guid GetGuid() const = 0;

        virtual uint64_t GetName(char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t GetVersion(char8_t* Version, uint64_t VersionSize) const = 0;

        virtual uint32_t GetVersionNumeric() const = 0;

        virtual OwningPtr<IArchive> OpenArchive(IArchive::FIPtr Interface) const = 0;

        virtual OwningPtr<IArchiveWritable> OpenArchiveWritable(IArchiveWritable::FIPtr Interface) const = 0;

        virtual OwningPtr<IUpstream> OpenUpstream(IUpstream::FIPtr Interface) const = 0;
    };

    static_assert(ISource::Version == SourceVersion::Latest);
}