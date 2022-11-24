s_project(L4::source::riot MODULE_LIB IS_SIMPLE PLATFORMS win)

s_link_libraries(PRIVATE L4::source)
s_compile_definitions(PRIVATE L4_SOURCE_BUILDING_SOURCE)

s_link_libraries(PRIVATE L4::format)
s_link_libraries(PRIVATE L4::streams)
s_link_libraries(PRIVATE L4::utils)
s_link_libraries(PRIVATE L4::web)
