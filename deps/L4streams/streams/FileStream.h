#pragma once

#include "Stream.h"

namespace L4
{
    class FileStream : public Stream
    {
#if defined(CONFIG_VERSION_PLATFORM_lnx)
        using HandleT = int;
#elif defined(CONFIG_VERSION_PLATFORM_win)
        using HandleT = void*;
#endif

    public:
        enum class OpenMode : uint8_t
        {
            Read = 0x1,
            Write = 0x2,
            ReadWrite = 0x3
        };

        enum class CreateMode : uint8_t
        {
            CreateOrOverwrite,
            CreateOrOpen,
            CreateOnly,
            OpenOnly,
            OverwriteOnly
        };

        enum class ShareMode : uint8_t
        {
            Exclusive,
            Read,
            Write,
            ReadWrite,
            Delete,
            DeleteRead,
            DeleteWrite,
            DeleteReadWrite
        };

        FileStream() noexcept;

        FileStream(const std::filesystem::path& Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode = ShareMode::Exclusive);

        FileStream(const std::string& Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode = ShareMode::Exclusive);

        FileStream(const wchar_t* Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode = ShareMode::Exclusive);

        FileStream(const char* Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode = ShareMode::Exclusive);

    private:
        FileStream(HandleT FileHandle);

    public:
        ~FileStream();

        FileStream(const FileStream&) = delete;

        FileStream(FileStream&&) noexcept;

        FileStream& operator=(const FileStream&) = delete;

        FileStream& operator=(FileStream&&) noexcept;

        bool IsValid() const;

        void WriteBytes(std::span<const std::byte> Src) override;

        void ReadBytes(std::span<std::byte> Dst) override;

        void Seek(ptrdiff_t Position, SeekPosition SeekFrom) override;

        size_t Tell() const override;

        size_t Size() const override;

        void Truncate();

    private:
        HandleT FileHandle;
    };
}