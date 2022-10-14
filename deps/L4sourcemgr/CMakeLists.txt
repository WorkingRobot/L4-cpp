L4_project(L4::sourcemgr STATIC_LIB IS_SIMPLE IS_DISABLED)

L4_link_libraries(PUBLIC L4::archive)
L4_link_libraries(PUBLIC libL4)
L4_link_libraries(PRIVATE L4::utils)
