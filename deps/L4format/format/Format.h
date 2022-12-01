#pragma once

#if 1
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#define FMT fmt
#else
#include <format>
#define FMT std
#endif