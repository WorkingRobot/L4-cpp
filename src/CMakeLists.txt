s_project(L4 EXEC IS_SIMPLE)

s_link_libraries(PRIVATE L4::manager)
s_link_libraries(PRIVATE L4::resources)

s_link_libraries(PRIVATE L4::modules::base)
s_link_libraries(PRIVATE L4::modules::plugins)
