#include "FileStream.h"

#include "utils/Error.win.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace L4
{
    static constexpr DWORD OpenModeToDesiredAccess(FileStream::OpenMode OpenMode)
    {
        switch (OpenMode)
        {
        case FileStream::OpenMode::Read:
            return GENERIC_READ;
        case FileStream::OpenMode::Write:
            return GENERIC_WRITE;
        case FileStream::OpenMode::ReadWrite:
            return GENERIC_READ | GENERIC_WRITE;
        default:
            throw std::invalid_argument("Invalid OpenMode");
        }
    }

    static constexpr DWORD CreateModeToCreationDisposition(FileStream::CreateMode CreateMode)
    {
        switch (CreateMode)
        {
        case FileStream::CreateMode::CreateOrOverwrite:
            return CREATE_ALWAYS;
        case FileStream::CreateMode::CreateOrOpen:
            return OPEN_ALWAYS;
        case FileStream::CreateMode::CreateOnly:
            return CREATE_NEW;
        case FileStream::CreateMode::OpenOnly:
            return OPEN_EXISTING;
        case FileStream::CreateMode::OverwriteOnly:
            return TRUNCATE_EXISTING;
        default:
            throw std::invalid_argument("Invalid CreateMode");
        }
    }

    static constexpr DWORD ShareModeToShareModeFlag(FileStream::ShareMode ShareMode)
    {
        switch (ShareMode)
        {
        case FileStream::ShareMode::Exclusive:
            return 0;
        case FileStream::ShareMode::Read:
            return FILE_SHARE_READ;
        case FileStream::ShareMode::Write:
            return FILE_SHARE_WRITE;
        case FileStream::ShareMode::ReadWrite:
            return FILE_SHARE_READ | FILE_SHARE_WRITE;
        case FileStream::ShareMode::Delete:
            return FILE_SHARE_DELETE;
        case FileStream::ShareMode::DeleteRead:
            return FILE_SHARE_DELETE | FILE_SHARE_READ;
        case FileStream::ShareMode::DeleteWrite:
            return FILE_SHARE_DELETE | FILE_SHARE_WRITE;
        case FileStream::ShareMode::DeleteReadWrite:
            return FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;
        default:
            throw std::invalid_argument("Invalid ShareMode");
        }
    }

    static constexpr DWORD SeekPositionToMoveMethod(FileStream::SeekPosition SeekPosition)
    {
        switch (SeekPosition)
        {
        case FileStream::SeekPosition::Beg:
            return FILE_BEGIN;
        case FileStream::SeekPosition::Cur:
            return FILE_CURRENT;
        case FileStream::SeekPosition::End:
            return FILE_END;
        default:
            throw std::invalid_argument("Invalid SeekPosition");
        }
    }

    FileStream::FileStream() noexcept :
        FileHandle(INVALID_HANDLE_VALUE)
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
        FileStream(CreateFileW(Path, OpenModeToDesiredAccess(OpenMode), ShareModeToShareModeFlag(ShareMode), NULL, CreateModeToCreationDisposition(CreateMode), FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    FileStream::FileStream(const char* Path, OpenMode OpenMode, CreateMode CreateMode, ShareMode ShareMode) :
        FileStream(CreateFileA(Path, OpenModeToDesiredAccess(OpenMode), ShareModeToShareModeFlag(ShareMode), NULL, CreateModeToCreationDisposition(CreateMode), FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    FileStream::FileStream(void* FileHandle) :
        FileHandle(FileHandle)
    {
        if (FileHandle == INVALID_HANDLE_VALUE)
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
    }

    FileStream::~FileStream()
    {
        CloseHandle(FileHandle);
    }

    FileStream::FileStream(FileStream&& Other) noexcept :
        FileHandle(std::exchange(Other.FileHandle, INVALID_HANDLE_VALUE))
    {
    }

    FileStream& FileStream::operator=(FileStream&& Other) noexcept
    {
        std::swap(FileHandle, Other.FileHandle);
        return *this;
    }

    bool FileStream::IsValid() const
    {
        return FileHandle != INVALID_HANDLE_VALUE;
    }

    void FileStream::WriteBytes(const std::byte* Src, size_t ByteCount)
    {
        do
        {
            DWORD CurrentByteCount = std::min<DWORD>(ByteCount, MAXDWORD);
            DWORD WriteByteCount = 0;

            if (!WriteFile(FileHandle, Src, CurrentByteCount, &WriteByteCount, NULL))
            {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }

            if (WriteByteCount != CurrentByteCount)
            {
                throw std::runtime_error("Could not fully satisfy write");
            }

            ByteCount -= WriteByteCount;
            Src += WriteByteCount;
        } while (ByteCount);
    }

    void FileStream::ReadBytes(std::byte* Dst, size_t ByteCount)
    {
        do
        {
            DWORD CurrentByteCount = std::min<DWORD>(ByteCount, MAXDWORD);
            DWORD ReadByteCount = 0;

            if (!ReadFile(FileHandle, Dst, CurrentByteCount, &ReadByteCount, NULL))
            {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }

            if (ReadByteCount != CurrentByteCount)
            {
                throw std::runtime_error("Could not fully satisfy read");
            }

            ByteCount -= ReadByteCount;
            Dst += ReadByteCount;
        } while (ByteCount);
    }

    void FileStream::Seek(ptrdiff_t Position, SeekPosition SeekFrom)
    {
        if (!SetFilePointerEx(FileHandle, LARGE_INTEGER { .QuadPart = Position }, NULL, SeekPositionToMoveMethod(SeekFrom)))
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
    }

    size_t FileStream::Tell() const
    {
        LARGE_INTEGER Position {};
        if (!SetFilePointerEx(FileHandle, LARGE_INTEGER { .QuadPart = 0 }, &Position, FILE_CURRENT))
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        return Position.QuadPart;
    }

    size_t FileStream::Size() const
    {
        LARGE_INTEGER Size {};
        if (!GetFileSizeEx(FileHandle, &Size))
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
        return Size.QuadPart;
    }

    void FileStream::Truncate()
    {
        Seek(0, Stream::Beg);
        if (!SetEndOfFile(FileHandle))
        {
            throw CreateErrorWin32(GetLastError(), __FUNCTION__);
        }
    }
}