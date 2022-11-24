s_project(L4::plugin::test MODULE_LIB IS_SIMPLE)

s_link_libraries(PRIVATE L4::plugin::wrapper)
s_link_libraries(PRIVATE L4::format)
s_add_dependency(OpenSSL PRIVATE)

# Report any undefined references
s_report_undefined_refs()