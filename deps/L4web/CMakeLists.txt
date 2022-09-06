L4_project(L4::web STATIC_LIB IS_SIMPLE)

L4_link_libraries(PUBLIC L4::log)

L4_add_dependency(Json INTERFACE)
L4_add_dependency(Http PRIVATE)

