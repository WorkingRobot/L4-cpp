#include "Source.h"

#include <source/Interface.h>

using namespace L4::Source::Riot;

extern "C" L4_API uint32_t GetSourceVersion()
{
    return (uint32_t)Source::Version;
}

extern "C" L4_API void* InitializeSource(uint32_t Version)
{
    if (Version != (uint32_t)Source::Version)
    {
        return nullptr;
    }
    return new Source;
}

extern "C" L4_API void UninitializeSource(void* SourcePtr)
{
    delete dynamic_cast<Source*>(reinterpret_cast<ISource*>(SourcePtr));
}