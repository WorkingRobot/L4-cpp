s_project(L4::plugin::manager STATIC_LIB)

s_add_dir_recursive(plugin)
s_end_sources()

s_link_libraries(PUBLIC L4::archive)
s_link_libraries(PRIVATE L4::utils)
s_link_libraries(PUBLIC libL4)
