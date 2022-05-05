#pragma once

#include <stdbool.h>
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

#ifndef L4_NOINTERFACE
#if defined(_WIN32) || defined(__WIN32__)
#define L4_EXPORT __declspec(dllexport)
#else
#define L4_EXPORT
#endif
#else
#define L4_EXPORT
#endif

typedef unsigned int L4_UInt32;
typedef unsigned char L4_Char8;
typedef bool L4_Bool;

typedef enum _L4_Version
{
    L4_VERSION_UNKNOWN,
    L4_VERSION_INITIAL,

    L4_VERSION_LATEST_PLUS_ONE,
    L4_VERSION_LATEST = L4_VERSION_LATEST_PLUS_ONE - 1,
    L4_VERSION_MAKE_32_BIT = 0xFFFFFFFF
} L4_Version;

typedef struct _L4_Guid
{
    L4_UInt32 A;
    L4_UInt32 B;
    L4_UInt32 C;
    L4_UInt32 D;
} L4_Guid;

typedef struct _L4_ArchiveMetadata
{
    L4_Guid Guid;
    L4_UInt32 VersionNumeric;
    L4_Char8 Name[40];
    L4_Char8 Version[60];
} L4_ArchiveMetadata;

typedef struct _L4_InitializeMetadata
{
} L4_InitializeMetadata;

typedef void* L4_FilePath;

typedef struct _L4_Interface
{
    L4_Version Version;
    L4_UInt32 Size;

    L4_Bool (*Initialize)(const L4_InitializeMetadata* Metadata);
    void (*Uninitialize)();

    const L4_ArchiveMetadata* (*GetArchiveMetadata)();

    L4_FilePath (*GetArchives)()
} L4_Interface;

L4_EXPORT const L4_Interface* GetInterface();

#if __cplusplus
}
#endif