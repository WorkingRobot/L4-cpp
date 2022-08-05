#pragma once

#include "../streams/Stream.h"
#include "../utils/Guid.h"
#include "../utils/HashCombine.h"

#include <L4/source/Structs.h>

namespace L4
{
    template <>
    struct Serializer<Source::Guid>
    {
        static void Serialize(Stream& Stream, const Source::Guid& Val)
        {
            Stream << Val.A;
            Stream << Val.B;
            Stream << Val.C;
            Stream << Val.D;
        }

        static void Deserialize(Stream& Stream, Source::Guid& Val)
        {
            Stream >> Val.A;
            Stream >> Val.B;
            Stream >> Val.C;
            Stream >> Val.D;
        }
    };

    static Source::String SerializeString(const std::u8string_view View)
    {
        return Source::String {
            .Data = View.data(),
            .Size = View.size()
        };
    }

    static std::u8string_view DeserializeString(const Source::String String)
    {
        return std::u8string_view(String.Data, String.Size);
    }

    static Source::Guid SerializeGuid(const Guid InputGuid)
    {
        return Source::Guid {
            .A = InputGuid.A,
            .B = InputGuid.B,
            .C = InputGuid.C,
            .D = InputGuid.D
        };
    }

    static Guid DeserializeGuid(const Source::Guid InputGuid)
    {
        return Guid {
            .A = InputGuid.A,
            .B = InputGuid.B,
            .C = InputGuid.C,
            .D = InputGuid.D
        };
    }
}

template <>
struct std::hash<L4::Source::Guid>
{
    std::size_t operator()(const L4::Source::Guid& Guid) const noexcept
    {
        return L4::HashCombine(Guid.A, Guid.B, Guid.C, Guid.D);
    }
};