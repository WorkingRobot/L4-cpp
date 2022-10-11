L4_project(L4::source::wrapper MODULE_LIB IS_SIMPLE)

L4_link_libraries(PRIVATE libL4)
L4_compile_definitions(PRIVATE L4_SOURCE_BUILDING_SOURCE)

