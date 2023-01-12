#pragma once

#include "Endian.h"

#include <format/Format.h>
#include <streams/Stream.h>

namespace L4::Plugin::FFXIV::ZiPatch
{
    struct SqpkFile
    {
        uint16_t MainId;
        uint16_t SubId;
        uint32_t FileId;

        void Deserialize(Stream& Stream)
        {
            MainId = Stream.Read<BE<uint16_t>>();
            SubId = Stream.Read<BE<uint16_t>>();
            FileId = Stream.Read<BE<uint32_t>>();
        }

        std::string GetExpansion() const
        {
            uint8_t ExpacId = SubId >> 8;
            if (ExpacId)
            {
                return fmt::format("ex{:d}", ExpacId);
            }
            else
            {
                return "ffxiv";
            }
        }

        std::string GetPlatform() const
        {
            return "win32";
        }

        std::string GetFilePath() const
        {
            return fmt::format("/sqpack/{:s}/{:02x}{:04x}.{:s}", GetExpansion(), MainId, SubId, GetPlatform());
        }

        std::string GetDatFilePath() const
        {
            return fmt::format("/sqpack/{:s}/{:02x}{:04x}.{:s}.dat{:d}", GetExpansion(), MainId, SubId, GetPlatform(), FileId);
        }

        std::string GetIndexFilePath() const
        {
            if (FileId)
            {
                return fmt::format("/sqpack/{:s}/{:02x}{:04x}.{:s}.index{:d}", GetExpansion(), MainId, SubId, GetPlatform(), FileId);
            }
            else
            {
                return fmt::format("/sqpack/{:s}/{:02x}{:04x}.{:s}.index", GetExpansion(), MainId, SubId, GetPlatform());
            }
        }
    };
}