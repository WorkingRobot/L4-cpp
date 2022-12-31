s_project(L4 EXEC IS_SIMPLE)

s_link_libraries(PRIVATE ext::type_name)
s_link_libraries(PRIVATE L4::gui)
s_link_libraries(PRIVATE L4::log)
s_link_libraries(PRIVATE L4::manager)
s_link_libraries(PRIVATE L4::resources)

s_link_libraries(PRIVATE L4::modules::base)
s_link_libraries(PRIVATE L4::modules::plugins)
