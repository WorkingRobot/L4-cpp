#pragma once

#include "Stream.h"

namespace L4
{
    class FileStream : public Stream
    {
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
        FileStream(void* FileHandle);

    public:
        ~FileStream();

        FileStream(const FileStream&) = delete;

        FileStream(FileStream&&) noexcept;

        FileStream& operator=(const FileStream&) = delete;

        FileStream& operator=(FileStream&&) noexcept;

        bool IsValid() const;

        void WriteBytes(const std::byte* Src, size_t ByteCount) override;

        void ReadBytes(std::byte* Dst, size_t ByteCount) override;

        void Seek(ptrdiff_t Position, SeekPosition SeekFrom) override;

        size_t Tell() const override;

        size_t Size() const override;

        void Truncate();

    private:
        void* FileHandle;
    };
}