L4_project(L4::plugin::manager STATIC_LIB)

L4_add_dir_recursive(plugin)
L4_end_sources()

L4_link_libraries(PUBLIC L4::archive)
L4_link_libraries(PRIVATE L4::utils)
L4_link_libraries(PUBLIC libL4)
