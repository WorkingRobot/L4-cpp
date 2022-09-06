L4_project(L4::utils STATIC_LIB IS_SIMPLE)

L4_add_version_defs(utils/Config.cpp)

if (PROJECT_PLATFORM STREQUAL "win")
    L4_link_libraries(INTERFACE ntdll)
elseif(PROJECT_PLATFORM STREQUAL "lnx")
    L4_add_dependency(libbacktrace PRIVATE)
    L4_add_dependency(breakpad PRIVATE)
    L4_add_dependency(date PUBLIC)
endif()
