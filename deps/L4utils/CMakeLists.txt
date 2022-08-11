L4_project_simple(L4::utils STATIC_LIB)

L4_add_version_defs(utils/Config.cpp)

L4_link_libraries(INTERFACE ntdll)