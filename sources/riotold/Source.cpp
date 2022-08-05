#include "Source.h"

#include "Archive.h"
#include "ArchiveWritable.h"
#include "Consts.h"
#include "Upstream.h"

#include <source/Error.h>

namespace L4::Source::Riot
{
    Guid Source::GetGuid() const
    {
        return SourceGuid;
    }

    uint64_t Source::GetName(char8_t* Name, uint64_t NameSize) const
    {
        SourceName.copy(Name, std::min(NameSize, SourceName.size()));
        return SourceName.size();
    }

    uint64_t Source::GetVersion(char8_t* Version, uint64_t VersionSize) const
    {
        SourceVersion.copy(Version, std::min(VersionSize, SourceVersion.size()));
        return SourceVersion.size();
    }

    uint32_t Source::GetVersionNumeric() const
    {
        return SourceVersionNumeric;
    }

    OwningPtr<IArchive> Source::OpenArchive(IArchive::FIPtr Interface) const
    {
        return CreateObject<Archive>(Interface);
    }

    OwningPtr<IArchiveWritable> Source::OpenArchiveWritable(IArchiveWritable::FIPtr Interface) const
    {
        return CreateObject<ArchiveWritable>(Interface);
    }

    OwningPtr<IUpstream> Source::OpenUpstream(IUpstream::FIPtr Interface) const
    {
        return CreateObject<Upstream>(Interface);
    }
}