s_project(L4::log STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ..)

s_link_libraries(PUBLIC L4::format)
s_link_libraries(PRIVATE L4::utils)
