#include <L4/source/Interface.h>
#include <L4/source/Structs.h>
#include <cstdio>

const L4::Source::L4Interface* L4Interface = nullptr;

const L4::Source::SourceInterface* Initialize(const L4::Source::L4Interface* Interface)
{
    L4Interface = Interface;
    printf("Initializing Riot Source with %s %s (%x)\n", (const char*)L4Interface->Identity.Name.Data, (const char*)L4Interface->Identity.Version.Humanized.Data, L4Interface->Identity.Version.Numeric);
    return nullptr;
}