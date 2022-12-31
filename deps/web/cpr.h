#pragma once

#include <cstdint>

// Prevents Windows.h from being included
#if defined(_WIN32) && !defined(_WINSOCKAPI_)
#define _WINSOCKAPI_
typedef uintptr_t SOCKET;
struct fd_set;
struct sockaddr
{
    uint16_t sa_family;
    char sa_data[14];
};
#endif
#include <cpr/session.h>