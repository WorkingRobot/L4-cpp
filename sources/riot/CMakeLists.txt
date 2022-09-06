L4_project(L4::source::riot MODULE_LIB IS_SIMPLE PLATFORMS win)

L4_link_libraries(PRIVATE L4::source)
L4_compile_definitions(PRIVATE L4_SOURCE_BUILDING_SOURCE)

L4_link_libraries(PRIVATE L4::formatters)
L4_link_libraries(PRIVATE L4::streams)
L4_link_libraries(PRIVATE L4::utils)
L4_link_libraries(PRIVATE L4::web)
