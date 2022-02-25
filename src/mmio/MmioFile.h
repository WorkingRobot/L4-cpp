#pragma once

#include <filesystem>

namespace L4
{
    class MmioFile
    {
    protected:
        using MM_HANDLE = void*;
        using MM_PVOID = void*;
        using MM_LARGE_INTEGER = long long;
        using MM_SIZE_T = size_t;

    public:
        MmioFile(const std::filesystem::path& Path);

        MmioFile(const std::string& Path);

        MmioFile(const wchar_t* Path);

        MmioFile(const char* Path);

    private:
        MmioFile(MM_HANDLE HFile);

    public:
        MmioFile(const MmioFile&) = delete;

        MmioFile(MmioFile&&) noexcept;

        ~MmioFile();

        std::filesystem::path GetPath() const;

        const void* GetBaseAddress() const noexcept;

        size_t GetSize() const noexcept;

    protected:
        MmioFile();

        void Flush(size_t Position, size_t Size);

        void Flush();

        MM_PVOID BaseAddress;
        MM_HANDLE HSection;
        MM_LARGE_INTEGER SectionSize;
        MM_SIZE_T ViewSize;
    };

    class MmioFileWritable : public MmioFile
    {
    public:
        MmioFileWritable(const std::filesystem::path& Path);

        MmioFileWritable(const std::string& Path);

        MmioFileWritable(const wchar_t* Path);

        MmioFileWritable(const char* Path);

    private:
        MmioFileWritable(MM_HANDLE HFile);

    public:
        void Reserve(size_t Size);

        void Flush(size_t Position, size_t Size);

        void Flush();
    };
}