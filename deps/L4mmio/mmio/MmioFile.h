#pragma once

#include <filesystem>

namespace L4::Mmio
{
    template <bool Writable>
    class MmioFileBase
    {
#if defined(CONFIG_VERSION_PLATFORM_lnx)
        using HandleT = int;
#elif defined(CONFIG_VERSION_PLATFORM_win)
        using HandleT = void*;
#endif

    public:
        MmioFileBase(const std::filesystem::path& Path);

        MmioFileBase(const std::string& Path);

        MmioFileBase(const wchar_t* Path);

        MmioFileBase(const char* Path);

    private:
        MmioFileBase(HandleT FileHandle);

    public:
        MmioFileBase(const MmioFileBase<Writable>&) = delete;

        MmioFileBase(MmioFileBase<Writable>&&) noexcept;

        ~MmioFileBase();

        const void* GetBaseAddress() const noexcept;

        void* GetBaseAddress() noexcept requires(Writable);

        size_t GetSize() const noexcept;

        template <class T = void>
        const T* Get(size_t ByteOffset = 0) const noexcept
        {
            return reinterpret_cast<const T*>(static_cast<const char*>(GetBaseAddress()) + ByteOffset);
        }

        template <class T = void>
        T* Get(size_t ByteOffset = 0) noexcept requires(Writable)
        {
            return reinterpret_cast<T*>(static_cast<char*>(GetBaseAddress()) + ByteOffset);
        }

        void Reserve(size_t Size) requires(Writable);

        void Flush(size_t Position = 0, size_t Size = -1) const requires(Writable);

    private:
#if defined(CONFIG_VERSION_PLATFORM_lnx)
        void* BaseAddress;
        int FileHandle;
        off_t FileSize;
#elif defined(CONFIG_VERSION_PLATFORM_win)
        using MM_HANDLE = HandleT;
        using MM_PVOID = void*;
        using MM_LARGE_INTEGER = long long;
        using MM_SIZE_T = size_t;

        MM_PVOID BaseAddress;
        MM_HANDLE HFile;
        MM_HANDLE HSection;
        MM_LARGE_INTEGER SectionSize;
        MM_SIZE_T ViewSize;
#endif
    };

    using MmioFile = MmioFileBase<false>;
    using MmioFileWritable = MmioFileBase<true>;
}