#pragma once

#include <chrono>

#if __cpp_lib_chrono < 201907
#include <date/tz.h>
#define TZ date
#else
#define TZ std::chrono
#endif
