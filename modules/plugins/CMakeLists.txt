s_project(L4::modules::plugins STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ../..)

s_link_libraries(PUBLIC L4::modules::base)
s_link_libraries(PUBLIC L4::manager)
s_link_libraries(PUBLIC L4::utils)
