s_project(L4::manager STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ..)

s_link_libraries(PUBLIC L4::utils)
s_link_libraries(PUBLIC libL4)
