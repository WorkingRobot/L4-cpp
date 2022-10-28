#pragma once

#if defined(CONFIG_VERSION_PLATFORM_lnx)
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#define FMT fmt
#elif defined(CONFIG_VERSION_PLATFORM_win)
#include <format>
#define FMT std
#endif