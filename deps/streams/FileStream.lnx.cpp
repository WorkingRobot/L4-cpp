#include "FileStream.h"

#include "utils/Error.lnx.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace L4
{
    static constexpr int OpenModeToAccessMode(FileStream::OpenMode OpenMode)
    {
        switch (OpenMode)
        {
        case FileStream::OpenMode::Read:
            return O_RDONLY;
        case FileStream::OpenMode::Write:
            return O_WRONLY;
        case FileStream::OpenMode::ReadWrite:
            return O_RDWR;
        default:
            throw std::invalid_argument("Invalid OpenMode");
        }
    }

    static constexpr int CreateModeToCreationFlags(FileStream::CreateMode CreateMode)
    {
        switch (CreateMode)
        {
        case FileStream::CreateMode::CreateOrOverwrite:
            return O_CREAT | O_TRUNC;
        case FileStream::CreateMode::CreateOrOpen:
            return O_CREAT;
        case FileStream::CreateMode::CreateOnly:
            return O_CREAT | O_EXCL;
        case FileStream::CreateMode::OpenOnly:
            return 0;
        case FileStream::CreateMode::OverwriteOnly:
            return O_TRUNC;
        default:
            throw std::invalid_argument("Invalid CreateMode");
        }
    }

    static constexpr int SeekPositionToWhence(FileStream::SeekPosition SeekPosition)
    {
        switch (SeekPosition)
        {
        case FileStream::SeekPosition::Beg:
            return SEEK_SET;
        case FileStream::SeekPosition::Cur:
            return SEEK_CUR;
        case FileStream::SeekPosition::End:
            return SEEK_END;
        default:
            throw std::invalid_argument("Invalid SeekPosition");
        }
    }

    FileStream::FileStream() noexcept :
        FileHandle(-1)
    {
    }

    FileStream::FileStream(const std::filesystem::path& Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode) :
        FileStream(Path.c_str(), OpenMode, CreateMode, ShareMode)
    {
    }

    FileStream::FileStream(const std::string& Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode) :
        FileStream(Path.c_str(), OpenMode, CreateMode, ShareMode)
    {
    }

    FileStream::FileStream(const wchar_t* Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode) :
        FileStream(std::filesystem::path(Path), OpenMode, CreateMode, ShareMode)
    {
    }

    // ShareMode is ignored in unix platforms as there is no equivalent; all files are shared between all authorized processes
    FileStream::FileStream(const char* Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode) :
        FileStream(open(Path, OpenModeToAccessMode(OpenMode) | CreateModeToCreationFlags(CreateMode)))
    {
    }

    FileStream::FileStream(int FileHandle) :
        FileHandle(FileHandle)
    {
        if (FileHandle == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    FileStream::~FileStream()
    {
        close(FileHandle);
    }

    FileStream::FileStream(FileStream&& Other) noexcept :
        FileHandle(std::exchange(Other.FileHandle, -1))
    {
    }

    FileStream& FileStream::operator=(FileStream&& Other) noexcept
    {
        std::swap(FileHandle, Other.FileHandle);
        return *this;
    }

    bool FileStream::IsValid() const
    {
        return FileHandle != -1;
    }

    void FileStream::WriteBytes(std::span<const std::byte> Src)
    {
        if (write(FileHandle, Src.data(), Src.size()) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    void FileStream::ReadBytes(std::span<std::byte> Dst)
    {
        if (read(FileHandle, Dst.data(), Data.size()) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    void FileStream::Seek(ptrdiff_t Position, SeekPosition SeekFrom)
    {
        if (lseek(FileHandle, Position, SeekPositionToWhence(SeekFrom)) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    size_t FileStream::Tell() const
    {
        off_t Position = lseek(FileHandle, 0, SEEK_CUR);
        if (Position == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
        return Position;
    }

    size_t FileStream::Size() const
    {
        struct stat StatBuf;
        if (fstat(FileHandle, &StatBuf) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
        return StatBuf.st_size;
    }

    void FileStream::Truncate()
    {
        if (ftruncate(FileHandle, 0) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        };
    }
}