#pragma once

#include "Archive.h"

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    class ArchiveWritable : public Archive, public IArchiveWritable
    {
        template <class T, class... ArgTs>
        friend OwningPtr<T> L4::Interface::CreateObject(ArgTs&&... Args);

    public:
        ArchiveWritable(IArchiveWritable::FIPtr Interface);

        Ptr<const ArchiveTree> GetTree() const final;

        uint64_t ReadTreeFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const final;
    };
}