s_project(L4::web STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ..)

s_link_libraries(PUBLIC L4::compat)
s_link_libraries(PUBLIC L4::log)

s_add_dependency(Json INTERFACE)
s_add_dependency(Http PUBLIC)

