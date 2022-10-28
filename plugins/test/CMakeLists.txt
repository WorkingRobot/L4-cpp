L4_project(L4::plugin::test MODULE_LIB IS_SIMPLE)

L4_link_libraries(PRIVATE L4::plugin::wrapper)
L4_link_libraries(PRIVATE L4::format)
L4_add_dependency(OpenSSL PRIVATE)

# Report any undefined references
L4_report_undefined_refs()