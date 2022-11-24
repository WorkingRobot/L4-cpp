#include "MmioFile.h"

#include "utils/Align.h"
#include "utils/Error.lnx.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace L4::Mmio
{
    static constexpr off_t ViewSizeIncrement = 1ull << 37; // 128 gb

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const std::filesystem::path& Path) :
        MmioFileBase(Path.c_str())
    {
    }

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const std::string& Path) :
        MmioFileBase(Path.c_str())
    {
    }

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const wchar_t* Path) :
        MmioFileBase(std::filesystem::path(Path))
    {
    }

    template <bool Writable>
    static constexpr int FileAccessMode =
        Writable ? O_RDWR : O_RDONLY;

    template <bool Writable>
    static constexpr int FileCreationFlags =
        Writable ? O_CREAT : 0;

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const char* Path) :
        MmioFileBase(open(Path, FileAccessMode<Writable> | FileCreationFlags<Writable>))
    {
    }

    template <bool Writable>
    static constexpr int MapPageProtection =
        Writable ? PROT_READ | PROT_WRITE : PROT_READ;

    template <bool Writable>
    static constexpr int MapPageFlags =
        Writable ? MAP_SHARED | MAP_FIXED : MAP_SHARED;

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(int FileHandle) :
        BaseAddress(nullptr),
        FileHandle(-1),
        FileSize(0)
    {
        this->FileHandle = FileHandle;

        struct stat StatBuf;
        if (fstat(FileHandle, &StatBuf) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
        FileSize = StatBuf.st_size;

        if constexpr (Writable)
        {
            if (FileSize == 0)
            {
                FileSize = 1;
            }
            // Align to page size so that no semifilled page exists at the end, just like how Windows does it
            FileSize = Align<0x1000>(FileSize);
            if (ftruncate(FileHandle, FileSize) == -1)
            {
                throw CreateErrorErrno(__FUNCTION__);
            }

            BaseAddress = mmap(nullptr, Align<ViewSizeIncrement>(FileSize), PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (BaseAddress == MAP_FAILED)
            {
                throw CreateErrorErrno(__FUNCTION__);
            }
        }

        BaseAddress = mmap(BaseAddress, FileSize, MapPageProtection<Writable>, MapPageFlags<Writable>, FileHandle, 0);
        if (BaseAddress == MAP_FAILED)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(MmioFileBase&& Other) noexcept :
        BaseAddress(std::exchange(Other.BaseAddress, nullptr)),
        FileHandle(std::exchange(Other.FileHandle, -1)),
        FileSize(std::exchange(Other.FileSize, 0))
    {
    }

    template <bool Writable>
    MmioFileBase<Writable>::~MmioFileBase()
    {
        if (BaseAddress)
        {
            if constexpr (Writable)
            {
                Flush();
            }
            munmap(BaseAddress, Writable ? Align<ViewSizeIncrement>(FileSize) : FileSize);
        }
        if (FileHandle != -1)
        {
            close(FileHandle);
        }
    }

    template <bool Writable>
    const void* MmioFileBase<Writable>::GetBaseAddress() const noexcept
    {
        return BaseAddress;
    }

    template <bool Writable>
    void* MmioFileBase<Writable>::GetBaseAddress() noexcept requires(Writable)
    {
        return BaseAddress;
    }

    template <bool Writable>
    size_t MmioFileBase<Writable>::GetSize() const noexcept
    {
        return FileSize;
    }

    template <bool Writable>
    void MmioFileBase<Writable>::Reserve(size_t Size) requires(Writable)
    {
        if (FileSize < Size)
        {
            Size = Align<0x1000>(Size);
            if (ftruncate(FileHandle, Size) == -1)
            {
                throw CreateErrorErrno(__FUNCTION__);
            }

            // Completely remap and change the base address
            // We don't unmap the previous address to prevent other threads from encountering an access error
            if (Align<ViewSizeIncrement>(Size) > Align<ViewSizeIncrement>(FileSize))
            {
                BaseAddress = mmap(nullptr, Align<ViewSizeIncrement>(Size), PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if (BaseAddress == MAP_FAILED)
                {
                    throw CreateErrorErrno(__FUNCTION__);
                }

                BaseAddress = mmap(BaseAddress, Size, MapPageProtection<Writable>, MapPageFlags<Writable>, FileHandle, 0);
                if (BaseAddress == MAP_FAILED)
                {
                    throw CreateErrorErrno(__FUNCTION__);
                }
            }
            else
            {
                void* Address = (std::byte*)BaseAddress + FileSize;
                Address = mmap(Address, Size - FileSize, MapPageProtection<Writable>, MapPageFlags<Writable>, FileHandle, FileSize);
                if (Address == MAP_FAILED)
                {
                    throw CreateErrorErrno(__FUNCTION__);
                }
            }
            FileSize = Size;
        }
    }

    template <bool Writable>
    void MmioFileBase<Writable>::Flush(size_t Position, size_t Size) const requires(Writable)
    {
        if (Position == 0 && Size == -1)
        {
            Size = FileSize;
        }
        if (msync((std::byte*)BaseAddress + Position, Size, MS_SYNC) == -1)
        {
            throw CreateErrorErrno(__FUNCTION__);
        }
    }

    template class MmioFileBase<false>;
    template class MmioFileBase<true>;
}
