#include "ArchiveCreator.h"

#include "ArchiveHandle.h"
#include "DirectoryTree.h"
#include "Entries.h"

namespace L4
{
    static constexpr Guid Source{ 0xb2a40d73, 0xe3dc9b49, 0x2bb6c045, 0x159c7b49 };
    static constexpr Guid App{ 0x109af169, 0xb653751a, 0xc0f3f83c, 0x1fbd7c50 };

    void CreateArchive(MmioFileWritable& File)
    {
        ArchiveHandle Handle(File.GetBaseAddress());
        File.Reserve(ClusterSize);

        DirectoryTreeClusterView MDT(Handle, 0u);
        auto Itr = MDT.begin();
        {
            auto Entry = EntryTraits<HeaderEntry>::Create();
            Entry.ClusterIdx = InvalidIdx;
            Entry.Flags = 0;
            Entry.StorageFlags = 0x80;
            Entry.SourceGuid = Source;
            Entry.AppGuid = App;
            Itr.set(Entry);
            ++Itr;
        }
        {
            auto Entry = EntryTraits<ClusterBitmapEntry>::Create();
            Entry.ClusterIdx = 1;
            Itr.set(Entry);
            ++Itr;
        }
        {
            auto Entry = EntryTraits<SectorTableEntry>::Create();
            Entry.ClusterIdx = 2;
            Itr.set(Entry);
            ++Itr;
        }
    }
}