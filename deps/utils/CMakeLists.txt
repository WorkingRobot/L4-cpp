s_project(L4::utils STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ..)

s_add_version_defs(Config.cpp)

if (PROJECT_PLATFORM STREQUAL "win")
    s_link_libraries(INTERFACE ext::ntdll)
endif()

if(PROJECT_PLATFORM STREQUAL "lnx")
    s_add_dependency(libbacktrace PRIVATE)
    s_add_dependency(breakpad PRIVATE)
    s_add_dependency(date PUBLIC)
endif()
