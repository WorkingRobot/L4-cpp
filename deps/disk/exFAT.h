#pragma once

#include "utils/IntervalTree.h"
#include "utils/Timezones.h"

#include <memory>
#include <string_view>

namespace L4
{
    enum FileAttributes : uint16_t
    {
        AttribReadOnly = 1 << 0,
        AttribHidden = 1 << 1,
        AttribSystem = 1 << 2,
        AttribDirectory = 1 << 4,
        AttribArchive = 1 << 5
    };

    using centiseconds = std::chrono::duration<std::chrono::milliseconds::rep, std::centi>;
    using ExFatTime = TZ::zoned_time<centiseconds>;

    struct ExFatEntry
    {
        std::u16string_view Name;
        FileAttributes Attributes;
        ExFatTime Created;
        ExFatTime Modified;
        ExFatTime Accessed;
    };

    struct ExFatFile : public ExFatEntry
    {
        IntervalList List;
        uint64_t DataLength;
    };

    struct ExFatDirectory : public ExFatEntry
    {
        std::vector<ExFatDirectory> Directories;
        std::vector<ExFatFile> Files;
    };

    static ExFatTime GetTime() noexcept
    {
        return { TZ::current_zone(), std::chrono::time_point_cast<centiseconds>(std::chrono::system_clock::now()) };
    }

    static ExFatDirectory CreateDirectory(
        const std::u16string_view Name,
        const std::initializer_list<ExFatDirectory> Directories = {},
        const std::initializer_list<ExFatFile> Files = {},
        FileAttributes Attributes = AttribDirectory,
        const ExFatTime* Created = nullptr,
        const ExFatTime* Modified = nullptr,
        const ExFatTime* Accessed = nullptr)
    {
        std::optional<ExFatTime> Time;
        if (!Created || !Modified || !Accessed)
        {
            Time = GetTime();
            if (!Created)
            {
                Created = &Time.value();
            }
            if (!Modified)
            {
                Modified = &Time.value();
            }
            if (!Accessed)
            {
                Accessed = &Time.value();
            }
        }

        Attributes = FileAttributes((Attributes & FileAttributes(~AttribArchive)) | AttribDirectory);

        return ExFatDirectory { Name, Attributes, *Created, *Modified, *Accessed, Directories, Files };
    }

    static ExFatFile CreateFile(
        const std::u16string_view Name,
        const IntervalList& List = {},
        uint64_t DataLength = 0,
        FileAttributes Attributes = AttribArchive,
        const ExFatTime* Created = nullptr,
        const ExFatTime* Modified = nullptr,
        const ExFatTime* Accessed = nullptr)
    {
        std::optional<ExFatTime> Time;
        if (!Created || !Modified || !Accessed)
        {
            Time = GetTime();
            if (!Created)
            {
                Created = &Time.value();
            }
            if (!Modified)
            {
                Modified = &Time.value();
            }
            if (!Accessed)
            {
                Accessed = &Time.value();
            }
        }

        Attributes = FileAttributes((Attributes & FileAttributes(~AttribDirectory)) | AttribArchive);

        return ExFatFile { Name, Attributes, *Created, *Modified, *Accessed, List, DataLength };
    }

    class ExFatSystem
    {
    public:
        ExFatSystem(uint64_t PartitionSectorOffset, uint64_t PartitionSectorCount, const ExFatDirectory& Tree);

        ~ExFatSystem();

        const IntervalList& GetIntervalList() const;

    private:
        class InternalData;
        std::unique_ptr<InternalData> Internal;
    };
}