s_project(L4::web STATIC_LIB IS_SIMPLE)

s_link_libraries(PUBLIC L4::log)

s_add_dependency(Json INTERFACE)
s_add_dependency(Http PRIVATE)

