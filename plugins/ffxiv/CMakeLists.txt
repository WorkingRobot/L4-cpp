s_project(L4::plugin::ffxiv MODULE_LIB IS_SIMPLE)

s_link_libraries(PUBLIC L4::plugin::wrapper)
s_link_libraries(PUBLIC L4::format)
s_link_libraries(PUBLIC L4::web)
s_link_libraries(PUBLIC L4::utils)
s_link_libraries(PRIVATE ext::steam)
s_link_libraries(PRIVATE L4::streams)

s_add_dependency(OpenSSL PRIVATE)

# Report any undefined references
if(NOT MSVC)
    s_report_undefined_refs()
endif()