#define L4_EXPORT
#include <SourceInterface.h>

using namespace L4::Interface;

class SourceRiot : public Source_1
{
    Guid GetGuid() const final
    {
        return { 0x42D44220, 0x572E6186, 0x7B14E7DC, 0x1C92ECBF };
    }

    Error OpenArchive(Archive** Archive, ArchiveFI* Interface)
    {
        return Error::UnsupportedArchive;
    }

    Error CloseArchive(Archive* Archive)
    {
        return Error::UnsupportedArchive; 
    }
};

extern "C" L4_API void* InitializeSource(uint32_t Version)
{
    if (Version != (uint32_t)SourceRiot::Version)
    {
        return nullptr;
    }
    return new SourceRiot;
}

extern "C" L4_API void UninitializeSource(void* Source)
{
    delete reinterpret_cast<SourceRiot*>(Source);
}