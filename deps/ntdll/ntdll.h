#pragma once

// clang-format off
// NOLINTBEGIN
#if __cplusplus
extern "C" {
#endif

#define WIN32_NO_STATUS
#define NOMINMAX
#include <Windows.h>
#undef WIN32_NO_STATUS

#include <ntstatus.h>
#include <winnt.h>

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

NTSTATUS NTAPI NtCreateSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL POBJECT_ATTRIBUTES ObjectAttributes,
    IN OPTIONAL PLARGE_INTEGER MaximumSize,
    IN ULONG SectionPageProtection,
    IN ULONG AllocationAttributes,
    IN OPTIONAL HANDLE FileHandle
);

NTSTATUS NTAPI NtMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID* BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN SIZE_T CommitSize,
    IN OUT OPTIONAL PLARGE_INTEGER SectionOffset,
    IN OUT PSIZE_T ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Win32Protect
);

NTSTATUS NTAPI NtExtendSection(
    IN HANDLE SectionHandle,
    IN OUT PLARGE_INTEGER NewSectionSize
);

NTSTATUS NTAPI NtUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
);

NTSTATUS NTAPI NtFlushVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID* BaseAddress,
    IN OUT PSIZE_T RegionSize,
    OUT PIO_STATUS_BLOCK IoStatus
);

NTSTATUS NTAPI NtClose(
    IN HANDLE Handle
);

ULONG NTAPI RtlNtStatusToDosError(
    IN NTSTATUS Status
);

#ifdef __cplusplus
}
#endif
// NOLINTEND
