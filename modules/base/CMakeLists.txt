s_project(L4::modules::base STATIC_LIB IS_SIMPLE INCLUDED_LOCATION ../..)

s_link_libraries(PUBLIC L4::gui)
s_link_libraries(PUBLIC L4::storage)
