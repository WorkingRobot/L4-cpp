#include "SourceArchive.h"

namespace L4
{
    SourceArchive::SourceArchive(ArchiveBase<true>&& Archive) :
        Archive(std::move(Archive))
    {
    }

    Source::AppIdentity SourceArchive::GetIdentity() const
    {
        auto& Header = Archive.GetHeader();

        return Source::AppIdentity {
            .App = {
                .Guid = SerializeGuid(Header.AppGuid),
                .Name = SerializeString(GetSV(Header.AppName)),
                .Version = {
                    .Humanized = SerializeString(GetSV(Header.AppVersion)),
                    .Numeric = Header.AppVersionNumeric
                }
            },
            .Source = {
                .Guid = SerializeGuid(Header.SourceGuid),
                .Name = SerializeString(GetSV(Header.SourceName)),
                .Version = {
                    .Humanized = SerializeString(GetSV(Header.SourceName)),
                    .Numeric = Header.SourceVersionNumeric
                }
            },
            .Environment = SerializeString(GetSV(Header.Environment))
        };
    }

    void SourceArchive::SetIdentity(const Source::AppIdentity& NewIdentity)
    {
        auto& Header = Archive.GetHeader();

        Header.AppGuid = DeserializeGuid(NewIdentity.App.Guid);
        SetSV(Header.AppName, DeserializeString(NewIdentity.App.Name));
        SetSV(Header.AppVersion, DeserializeString(NewIdentity.App.Version.Humanized));
        Header.AppVersionNumeric = NewIdentity.App.Version.Numeric;

        Header.SourceGuid = DeserializeGuid(NewIdentity.Source.Guid);
        SetSV(Header.SourceName, DeserializeString(NewIdentity.Source.Name));
        SetSV(Header.SourceVersion, DeserializeString(NewIdentity.Source.Version.Humanized));
        Header.SourceVersionNumeric = NewIdentity.Source.Version.Numeric;

        SetSV(Header.Environment, DeserializeString(NewIdentity.Environment));
    }

    uint32_t SourceArchive::GetStreamCount() const
    {
        return Archive.GetStreamCount();
    }

    uint32_t SourceArchive::GetSectorSize() const
    {
        return Archive.GetSectorSize();
    }

    uint32_t SourceArchive::GetStreamIdxFromGuid(Source::Guid Guid) const
    {
        uint32_t StreamCount = Archive.GetStreamCount();
        for (uint32_t Idx = 0; Idx < StreamCount; ++Idx)
        {
            auto& Header = Archive.GetStreamHeader(Idx);
            if (SerializeGuid(Header.Guid) == Guid)
            {
                return Idx;
            }
        }

        return -1;
    }

    SourceStream* SourceArchive::OpenStreamRead(uint32_t StreamIdx)
    {
        if (StreamIdx >= Archive.GetStreamCount())
        {
            return nullptr;
        }

        return Streams.emplace_back(std::make_unique<SourceStream>(StreamViewWritable(Archive, StreamIdx))).get();
    }

    SourceStream* SourceArchive::OpenStreamWrite(uint32_t StreamIdx)
    {
        if (StreamIdx >= Archive.GetStreamCount())
        {
            return nullptr;
        }

        return Streams.emplace_back(std::make_unique<SourceStream>(StreamViewWritable(Archive, StreamIdx))).get();
    }

    void SourceArchive::CloseStream(const SourceStream& Stream)
    {
        std::erase_if(Streams, [&](const auto& StreamPtr) { return StreamPtr.get() == &Stream; });
    }
}