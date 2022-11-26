s_project(L4::plugin::ffxiv MODULE_LIB IS_SIMPLE)

s_link_libraries(PRIVATE L4::plugin::wrapper)
s_link_libraries(PRIVATE L4::format)
s_link_libraries(PRIVATE L4::web)

# Report any undefined references
s_report_undefined_refs()