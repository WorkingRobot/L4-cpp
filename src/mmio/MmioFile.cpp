#include "MmioFile.h"

#include "../utils/Align.h"
#include "../utils/Error.h"

#include <ntdll.h>
#include <Psapi.h>

namespace L4 {
    // Formats GetMappedFileName into something more readable
    static bool ResolveDevicePath(std::string& Filename)
    {
        char DriveLetters[512];
        if (GetLogicalDriveStrings(sizeof(DriveLetters) - 1, DriveLetters)) {
            char DriveName[MAX_PATH];
            char Drive[3] = " :";
            for (auto DriveLetter = DriveLetters; *DriveLetter; DriveLetter += strlen(DriveLetter) + 1) {
                Drive[0] = *DriveLetter;
                if (QueryDosDevice(Drive, DriveName, MAX_PATH)) {
                    size_t DriveNameLen = strlen(DriveName);

                    if (DriveNameLen < MAX_PATH) {
                        if (Filename.starts_with({ DriveName, DriveNameLen }) && Filename[DriveNameLen] == '\\') {
                            Filename.replace(0, DriveNameLen, Drive);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    static constexpr SIZE_T ViewSizeIncrement = 1ull << 37; // 128 gb

    MmioFile::MmioFile(const std::filesystem::path& Path) :
        MmioFile(Path.c_str())
    {
    }

    MmioFile::MmioFile(const std::string& Path) :
        MmioFile(Path.c_str())
    {
    }

    MmioFile::MmioFile(const wchar_t* Path) :
        MmioFile(CreateFileW(Path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    MmioFile::MmioFile(const char* Path) :
        MmioFile(CreateFileA(Path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    MmioFile::MmioFile(MM_HANDLE HFile) :
        MmioFile()
    {
        this->HFile = HFile;

        NTSTATUS Status;
        {
            if (HFile == INVALID_HANDLE_VALUE) {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }
            if (!GetFileSizeEx(HFile, (PLARGE_INTEGER)&SectionSize)) {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }

            // Note: NtCreateSection can return STATUS_MAPPED_FILE_SIZE_ZERO if the file is empty
            Status = NtCreateSection(&HSection, SECTION_MAP_READ, NULL, (PLARGE_INTEGER)&SectionSize, PAGE_READONLY, SEC_COMMIT, HFile);
            if (Status != STATUS_SUCCESS) {
                throw CreateErrorNtStatus(Status, __FUNCTION__);
            }
        }

        ViewSize = SectionSize;
        Status = NtMapViewOfSection(HSection, GetCurrentProcess(), &BaseAddress, 0, 0, NULL, &ViewSize, ViewUnmap, 0, PAGE_READONLY);
        if (Status != STATUS_SUCCESS) {
            throw CreateErrorNtStatus(Status, __FUNCTION__);
        }
    }

    MmioFile::MmioFile(MmioFile&& Other) noexcept :
        BaseAddress(std::exchange(Other.BaseAddress, nullptr)),
        HFile(std::exchange(Other.HFile, INVALID_HANDLE_VALUE)),
        HSection(std::exchange(Other.HSection, INVALID_HANDLE_VALUE)),
        SectionSize(std::exchange(Other.SectionSize, 0)),
        ViewSize(std::exchange(Other.ViewSize, 0))
    {
    }

    MmioFile::~MmioFile()
    {
        if (BaseAddress) {
            Flush();
            NtUnmapViewOfSection(GetCurrentProcess(), BaseAddress);
            EmptyWorkingSet(GetCurrentProcess());
        }
        if (HSection != INVALID_HANDLE_VALUE) {
            NtClose(HSection);
        }
        if (HFile != INVALID_HANDLE_VALUE) {
            NtClose(HFile);
        }
    }

    std::filesystem::path MmioFile::GetPath() const
    {
        char Filename[MAX_PATH] {};
        if (GetMappedFileName(GetCurrentProcess(), BaseAddress, Filename, MAX_PATH)) {
            std::string FilenameString = Filename;
            if (ResolveDevicePath(FilenameString)) {
                return FilenameString;
            }
        }
        return "";
    }

    const void* MmioFile::GetBaseAddress() const noexcept
    {
        return BaseAddress;
    }

    size_t MmioFile::GetSize() const noexcept
    {
        return SectionSize;
    }

    MmioFile::MmioFile() noexcept :
        BaseAddress(nullptr),
        HFile(INVALID_HANDLE_VALUE),
        HSection(INVALID_HANDLE_VALUE),
        SectionSize(0),
        ViewSize(0)
    {
    }

    void MmioFile::Flush(size_t Position, size_t Size) const
    {
        VirtualUnlock((LPBYTE)BaseAddress + Position, Size);
    }

    void MmioFile::Flush() const
    {
        SIZE_T FlushSize = 0;
        PVOID FlushAddr = BaseAddress;
        IO_STATUS_BLOCK Block;
        NtFlushVirtualMemory(GetCurrentProcess(), &FlushAddr, &FlushSize, &Block);
    }

    MmioFileWritable::MmioFileWritable(const std::filesystem::path& Path) :
        MmioFileWritable(Path.c_str())
    {
    }

    MmioFileWritable::MmioFileWritable(const std::string& Path) :
        MmioFileWritable(Path.c_str())
    {
    }

    MmioFileWritable::MmioFileWritable(const wchar_t* Path) :
        MmioFileWritable(CreateFileW(Path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    MmioFileWritable::MmioFileWritable(const char* Path) :
        MmioFileWritable(CreateFileA(Path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
    {
    }

    MmioFileWritable::MmioFileWritable(MM_HANDLE HFile) :
        MmioFile()
    {
        this->HFile = HFile;

        NTSTATUS Status;
        {
            if (HFile == INVALID_HANDLE_VALUE) {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }
            if (!GetFileSizeEx(HFile, (PLARGE_INTEGER)&SectionSize)) {
                throw CreateErrorWin32(GetLastError(), __FUNCTION__);
            }

            // NtCreateSection will return STATUS_MAPPED_FILE_SIZE_ZERO otherwise
            if (SectionSize == 0) {
                SectionSize = 1;
            }

            Status = NtCreateSection(&HSection, SECTION_EXTEND_SIZE | SECTION_MAP_READ | SECTION_MAP_WRITE, NULL, (PLARGE_INTEGER)&SectionSize, PAGE_READWRITE, SEC_COMMIT, HFile);
            if (Status != STATUS_SUCCESS) {
                throw CreateErrorNtStatus(Status, __FUNCTION__);
            }
        }

        ViewSize = Align<ViewSizeIncrement>(SectionSize);
        Status = NtMapViewOfSection(HSection, GetCurrentProcess(), &BaseAddress, 0, 0, NULL, &ViewSize, ViewUnmap, MEM_RESERVE, PAGE_READWRITE);
        if (Status != STATUS_SUCCESS) {
            throw CreateErrorNtStatus(Status, __FUNCTION__);
        }
    }

    void* MmioFileWritable::GetBaseAddress() const noexcept
    {
        return BaseAddress;
    }

    void MmioFileWritable::Reserve(size_t Size)
    {
        if (SectionSize < Size) {
            SectionSize = Size;
            NTSTATUS Status = NtExtendSection(HSection, (PLARGE_INTEGER)&SectionSize);
            if (Status != STATUS_SUCCESS) {
                throw CreateErrorNtStatus(Status, __FUNCTION__);
            }

            if (Size > ViewSize) {
                ViewSize = Align<ViewSizeIncrement>(Size);

                // By not unmapping, we prevent code that is running in other threads from encountering an access error
                /*Status = NtUnmapViewOfSection(GetCurrentProcess(), BaseAddress);
                if (Status != STATUS_SUCCESS)
                {
                    throw CreateErrorNtStatus(Status, __FUNCTION__);
                }*/

                BaseAddress = NULL;
                Status = NtMapViewOfSection(HSection, GetCurrentProcess(), &BaseAddress, 0, 0, NULL, &ViewSize, ViewUnmap, MEM_RESERVE, PAGE_READWRITE);
                if (Status != STATUS_SUCCESS) {
                    throw CreateErrorNtStatus(Status, __FUNCTION__);
                }
            }
        }
    }

    void MmioFileWritable::Flush(size_t Position, size_t Size) const
    {
        MmioFile::Flush(Position, Size);
    }

    void MmioFileWritable::Flush() const
    {
        MmioFile::Flush();
    }
}
