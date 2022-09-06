L4_project(L4::disk STATIC_LIB IS_SIMPLE)

L4_link_libraries(PRIVATE L4::utils)
L4_add_dependency(date PUBLIC)
