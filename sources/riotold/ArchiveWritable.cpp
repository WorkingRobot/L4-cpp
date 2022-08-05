#include "ArchiveWritable.h"

namespace L4::Source::Riot
{
    ArchiveWritable::ArchiveWritable(IArchiveWritable::FIPtr Interface) :
        Archive(Interface),
        IArchiveWritable(Interface)
    {
    }

    Ptr<const ArchiveTree> ArchiveWritable::GetTree() const
    {
        return Archive::GetTree();
    }

    uint64_t ArchiveWritable::ReadTreeFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const
    {
        return Archive::ReadTreeFile(Context, Offset, Data, DataSize);
    }
}