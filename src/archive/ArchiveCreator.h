#pragma once

#include "../mmio/MmioFile.h"
#include "../utils/Align.h"
#include "Freelist.h"
#include "Header.h"
#include "StreamHeader.h"
#include "StreamRunlist.h"

#include <span>

namespace L4 {
    struct SectorSize {
        uint32_t Size;
    };

    struct SourceInfo {
        std::u8string_view Name;
        std::u8string_view Version;
        uint32_t VersionNumeric;
        Guid Guid;
    };

    struct AppInfo {
        std::u8string_view Name;
        std::u8string_view Version;
        uint32_t VersionNumeric;
        Guid Guid;
    };

    struct SingleStreamInfo {
        Guid Guid;
        uint32_t Version;
        uint32_t ElementSize;
        std::u8string_view Name;
        std::span<std::byte, 192> Context;
    };

    using StreamInfo = std::initializer_list<SingleStreamInfo>;

    namespace Detail {
        class ArchiveCreator {
        public:
            template <class... ArgTs>
            ArchiveCreator(MmioFileWritable& File, ArgTs&&... Args) :
                File(File)
            {
                File.Reserve(sizeof(Header));

                std::initializer_list<int> ignore = { (Set(std::forward<ArgTs>(Args)), 0)... };
                (void)ignore;
            }

            void Set(const SectorSize& SectorSize)
            {
                if (!std::has_single_bit(SectorSize.Size)) {
                    throw std::invalid_argument("Sector size must be a power of two");
                }
                if (SectorSize.Size < 4096) {
                    throw std::invalid_argument("Sector size must be at least 4 KiB");
                }

                auto& Header = *File.Get<L4::Header>();
                Header.SectorSize = SectorSize.Size;
            }

            void Set(const SourceInfo& SourceInfo)
            {
                auto& Header = *File.Get<L4::Header>();
                SetSV(Header.SourceName, SourceInfo.Name);
                SetSV(Header.SourceVersion, SourceInfo.Version);
                Header.SourceVersionNumeric = SourceInfo.VersionNumeric;
                Header.SourceGuid = SourceInfo.Guid;
            }

            void Set(const AppInfo& AppInfo)
            {
                auto& Header = *File.Get<L4::Header>();
                SetSV(Header.AppName, AppInfo.Name);
                SetSV(Header.AppVersion, AppInfo.Version);
                Header.AppVersionNumeric = AppInfo.VersionNumeric;
                Header.AppGuid = AppInfo.Guid;
            }

            void Set(const StreamInfo& StreamInfo)
            {
                {
                    auto& Header = *File.Get<L4::Header>();
                    Header.StreamCount = StreamInfo.size();
                }

                File.Reserve(sizeof(L4::Header) + StreamInfo.size() * sizeof(L4::StreamHeader));
                StreamHeader* Header = File.Get<StreamHeader>(sizeof(L4::Header));
                for (auto& Info : StreamInfo) {
                    Header->Guid = Info.Guid;
                    Header->Version = Info.Version;
                    Header->ElementSize = Info.ElementSize;
                    SetSV(Header->Name, Info.Name);
                    std::ranges::copy(Info.Context, Header->Context);
                    ++Header;
                }
            }

            void Build()
            {
                auto& Header = *File.Get<L4::Header>();
                Header.Magic = ExpectedMagic;
                Header.Version = ArchiveVersion::Latest;

                auto FreelistOffset = Align(sizeof(L4::Header) + Header.StreamCount * sizeof(L4::StreamHeader), Header.SectorSize);
                auto AllocatedSize = FreelistOffset + Align(sizeof(Freelist) + Header.StreamCount * sizeof(L4::StreamRunlist), Header.SectorSize);
                File.Reserve(AllocatedSize);

                Freelist& Freelist = *File.Get<L4::Freelist>(FreelistOffset);
                Freelist.TotalSectorCount = AllocatedSize / Header.SectorSize;

                std::fill_n(File.Get<StreamRunlist>(FreelistOffset + sizeof(Freelist)), Header.StreamCount, StreamRunlist {});
            }

        protected:
            MmioFileWritable& File;
        };
    }

    template <class... ArgTs>
    void CreateArchive(MmioFileWritable& File, ArgTs&&... Args)
    {
        Detail::ArchiveCreator Creator(File, std::forward<ArgTs>(Args)...);
        Creator.Build();
    }
}