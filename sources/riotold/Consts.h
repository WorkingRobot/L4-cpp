#include <source/Guid.h>

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    static constexpr Guid SourceGuid { 0x42D44220, 0x572E6186, 0x7B14E7DC, 0x1C92ECBF };
    static constexpr std::u8string_view SourceName = u8"Riot Games";
    static constexpr std::u8string_view SourceVersion = u8"v0.1.0";
    static constexpr uint32_t SourceVersionNumeric = 0;
}