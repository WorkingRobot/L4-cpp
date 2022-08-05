#pragma once

#include <source/Source.h>

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    class Source : public ISource
    {
    public:
        Guid GetGuid() const final;

        uint64_t GetName(char8_t* Name, uint64_t NameSize) const final;

        uint64_t GetVersion(char8_t* Version, uint64_t VersionSize) const final;

        uint32_t GetVersionNumeric() const final;

        OwningPtr<IArchive> OpenArchive(IArchive::FIPtr Interface) const final;

        OwningPtr<IArchiveWritable> OpenArchiveWritable(IArchiveWritable::FIPtr Interface) const final;

        OwningPtr<IUpstream> OpenUpstream(IUpstream::FIPtr Interface) const final;
    };
}