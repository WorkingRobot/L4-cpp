s_project(L4::utils STATIC_LIB IS_SIMPLE)

s_add_version_defs(utils/Config.cpp)

if (PROJECT_PLATFORM STREQUAL "win")
    s_link_libraries(INTERFACE ntdll)
elseif(PROJECT_PLATFORM STREQUAL "lnx")
    s_add_dependency(libbacktrace PRIVATE)
    s_add_dependency(breakpad PRIVATE)
    s_add_dependency(date PUBLIC)
endif()
