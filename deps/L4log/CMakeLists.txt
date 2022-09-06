L4_project(L4::log STATIC_LIB IS_SIMPLE)

L4_link_libraries(PRIVATE L4::formatters)
L4_link_libraries(PRIVATE L4::utils)

if (PROJECT_PLATFORM STREQUAL "lnx")
    L4_add_dependency(libfmt PUBLIC)
endif()
