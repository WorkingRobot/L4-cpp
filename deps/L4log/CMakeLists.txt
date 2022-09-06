L4_project(L4::log STATIC_LIB IS_SIMPLE)

L4_link_libraries(PUBLIC L4::format)
L4_link_libraries(PRIVATE L4::utils)
