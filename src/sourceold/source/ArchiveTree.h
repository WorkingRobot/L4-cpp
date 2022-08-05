#pragma once

#include <span>
#include <string_view>
#include <vector>

namespace L4::Interface
{
    struct ArchiveTreeNode
    {
        std::u8string_view Name;
        uint64_t Created;
        uint64_t Modified;
        uint64_t Accessed;
        uint16_t Hidden : 1;
        uint16_t System : 1;
    };

    enum class FileReadStrategy : uint8_t
    {
        Unknown,
        Direct,
        Callback,
        IntervalDirect,
        IntervalCallback
    };

    struct ArchivePosition
    {
        uint32_t StreamIdx;
        uint64_t StreamOffset;
    };

    // [Start, End]
    template <class CtxT>
    struct FileInterval
    {
        uint64_t Start;
        uint64_t End;
        CtxT Context;
    };

    struct ArchiveTreeFile : public ArchiveTreeNode
    {
        uint64_t FileSize;
        FileReadStrategy ReadStrategy;
        union ReadStrategyData_
        {
            std::monostate Unknown;
            ArchivePosition Direct;
            void* Callback;
            std::span<FileInterval<ArchivePosition>> IntervalDirect;
            std::span<FileInterval<void*>> IntervalCallback;

            ReadStrategyData_() :
                Unknown() {};
        } ReadStrategyData;
    };

    struct ArchiveTreeDirectory : public ArchiveTreeNode
    {
        std::span<ArchiveTreeDirectory> Directories;
        std::span<ArchiveTreeFile> Files;
    };

    struct ArchiveTree
    {
        std::span<ArchiveTreeDirectory> Directories;
        std::span<ArchiveTreeFile> Files;
        std::string_view Timezone;
    };
}