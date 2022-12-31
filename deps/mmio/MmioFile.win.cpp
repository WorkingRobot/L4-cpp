#include "MmioFile.h"

#include "utils/Align.h"
#include "utils/Error.win.h"

#include <ntdll.h>
#include <Psapi.h>

namespace L4::Mmio
{
    static constexpr SIZE_T ViewSizeIncrement = 1ull << 37; // 128 gb

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
    static constexpr DWORD FileDesiredAccess =
        Writable ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ;

    template <bool Writable>
    static constexpr DWORD FileCreationDisposition =
        Writable ? OPEN_ALWAYS : OPEN_EXISTING;

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const wchar_t* Path) :
        MmioFileBase(CreateFileW(Path, FileDesiredAccess<Writable>, 0, NULL, FileCreationDisposition<Writable>, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(const char* Path) :
        MmioFileBase(CreateFileA(Path, FileDesiredAccess<Writable>, 0, NULL, FileCreationDisposition<Writable>, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    template <bool Writable>
    static constexpr DWORD SectionDesiredAccess =
        Writable ? SECTION_EXTEND_SIZE | SECTION_MAP_READ | SECTION_MAP_WRITE : SECTION_MAP_READ;

    template <bool Writable>
    static constexpr DWORD SectionPageProtection =
        Writable ? PAGE_READONLY : PAGE_READWRITE;

    template <bool Writable>
    static constexpr ULONG SectionAllocationType =
        Writable ? MEM_RESERVE : 0;

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(MM_HANDLE HFile) :
        BaseAddress(nullptr),
        HFile(INVALID_HANDLE_VALUE),
        HSection(INVALID_HANDLE_VALUE),
        SectionSize(0),
        ViewSize(0)
    {
        this->HFile = HFile;

        NTSTATUS Status;
        {
            if (HFile == INVALID_HANDLE_VALUE)
            {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }
            if (!GetFileSizeEx(HFile, (PLARGE_INTEGER)&SectionSize))
            {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }

            if constexpr (Writable)
            {
                // NtCreateSection will return STATUS_MAPPED_FILE_SIZE_ZERO otherwise
                if (SectionSize == 0)
                {
                    SectionSize = 1;
                }
            }

            // Note: NtCreateSection can return STATUS_MAPPED_FILE_SIZE_ZERO if the file is empty
            Status = NtCreateSection(&HSection, SectionDesiredAccess<Writable>, NULL, (PLARGE_INTEGER)&SectionSize, SectionPageProtection<Writable>, SEC_COMMIT, HFile);
            if (Status != STATUS_SUCCESS)
            {
                throw CreateErrorNtStatus(Status, __FUNCTION__);
            }
        }

        if constexpr (Writable)
        {
            ViewSize = Align<ViewSizeIncrement>(SectionSize);
        }
        else
        {
            ViewSize = SectionSize;
        }
        Status = NtMapViewOfSection(HSection, GetCurrentProcess(), &BaseAddress, 0, 0, NULL, &ViewSize, ViewUnmap, SectionAllocationType<Writable>, SectionPageProtection<Writable>);
        if (Status != STATUS_SUCCESS)
        {
            throw CreateErrorNtStatus(Status, __FUNCTION__);
        }
    }

    template <bool Writable>
    MmioFileBase<Writable>::MmioFileBase(MmioFileBase&& Other) noexcept :
        BaseAddress(std::exchange(Other.BaseAddress, nullptr)),
        HFile(std::exchange(Other.HFile, INVALID_HANDLE_VALUE)),
        HSection(std::exchange(Other.HSection, INVALID_HANDLE_VALUE)),
        SectionSize(std::exchange(Other.SectionSize, 0)),
        ViewSize(std::exchange(Other.ViewSize, 0))
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
            NtUnmapViewOfSection(GetCurrentProcess(), BaseAddress);
            EmptyWorkingSet(GetCurrentProcess());
        }
        if (HSection != INVALID_HANDLE_VALUE)
        {
            NtClose(HSection);
        }
        if (HFile != INVALID_HANDLE_VALUE)
        {
            NtClose(HFile);
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
        return SectionSize;
    }

    template <bool Writable>
    void MmioFileBase<Writable>::Reserve(size_t Size) requires(Writable)
    {
        if (SectionSize < Size)
        {
            SectionSize = Size;
            NTSTATUS Status = NtExtendSection(HSection, (PLARGE_INTEGER)&SectionSize);
            if (Status != STATUS_SUCCESS)
            {
                throw CreateErrorNtStatus(Status, __FUNCTION__);
            }

            if (Size > ViewSize)
            {
                ViewSize = Align<ViewSizeIncrement>(Size);

                // By not unmapping, we prevent code that is running in other threads from encountering an access error
                /*Status = NtUnmapViewOfSection(GetCurrentProcess(), BaseAddress);
                if (Status != STATUS_SUCCESS)
                {
                    throw CreateErrorNtStatus(Status, __FUNCTION__);
                }*/

                BaseAddress = NULL;
                Status = NtMapViewOfSection(HSection, GetCurrentProcess(), &BaseAddress, 0, 0, NULL, &ViewSize, ViewUnmap, MEM_RESERVE, PAGE_READWRITE);
                if (Status != STATUS_SUCCESS)
                {
                    throw CreateErrorNtStatus(Status, __FUNCTION__);
                }
            }
        }
    }

    template <bool Writable>
    void MmioFileBase<Writable>::Flush(size_t Position, size_t Size) const requires(Writable)
    {
        if (Position == 0 && Size == -1)
        {
            SIZE_T FlushSize = 0;
            PVOID FlushAddr = BaseAddress;
            IO_STATUS_BLOCK Block;
            NtFlushVirtualMemory(GetCurrentProcess(), &FlushAddr, &FlushSize, &Block);
        }
        else
        {
            VirtualUnlock((LPBYTE)BaseAddress + Position, Size);
        }
    }

    template class MmioFileBase<false>;
    template class MmioFileBase<true>;
}
