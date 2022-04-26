#pragma once

#include "../tree/IntervalTree.h"
#include <memory>
#include <chrono>

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

    using centiseconds = std::chrono::duration<long long, std::centi>;
    using ExFatTime = std::chrono::zoned_time<centiseconds>;

    struct ExFatEntry
    {
        std::wstring_view Name;
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