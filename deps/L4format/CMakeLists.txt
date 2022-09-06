L4_project(L4::format HEADER_LIB IS_SIMPLE)

if (PROJECT_PLATFORM STREQUAL "lnx")
    L4_add_dependency(libfmt INTERFACE)
endif()
 