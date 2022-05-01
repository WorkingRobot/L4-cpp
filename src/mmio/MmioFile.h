#pragma once

#include <filesystem>

namespace L4 {
    template <bool Writable>
    class MmioFileBase {
        using MM_HANDLE = void*;
        using MM_PVOID = void*;
        using MM_LARGE_INTEGER = long long;
        using MM_SIZE_T = size_t;

    public:
        MmioFileBase(const std::filesystem::path& Path);

        MmioFileBase(const std::string& Path);

        MmioFileBase(const wchar_t* Path);

        MmioFileBase(const char* Path);

    private:
        MmioFileBase(MM_HANDLE HFile);

    public:
        MmioFileBase(const MmioFileBase<Writable>&) = delete;

        MmioFileBase(MmioFileBase<Writable>&&) noexcept;

        ~MmioFileBase();

        std::filesystem::path GetPath() const;

        const void* GetBaseAddress() const noexcept;

        void* GetBaseAddress() const noexcept requires(Writable);

        size_t GetSize() const noexcept;

        template <class T = void>
        const T* Get(size_t ByteOffset = 0) const noexcept
        {
            return reinterpret_cast<const T*>(static_cast<const char*>(GetBaseAddress()) + ByteOffset);
        }

        template <class T = void>
        T* Get(size_t ByteOffset = 0) const noexcept requires(Writable)
        {
            return reinterpret_cast<T*>(static_cast<char*>(GetBaseAddress()) + ByteOffset);
        }

        void Reserve(size_t Size) requires(Writable);

        void Flush(size_t Position, size_t Size) const requires(Writable);

        void Flush() const requires(Writable);

    private:
        MM_PVOID BaseAddress;
        MM_HANDLE HFile;
        MM_HANDLE HSection;
        MM_LARGE_INTEGER SectionSize;
        MM_SIZE_T ViewSize;
    };

    using MmioFile = MmioFileBase<false>;
    using MmioFileWritable = MmioFileBase<true>;
}