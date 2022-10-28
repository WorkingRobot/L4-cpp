L4_project(L4::plugin::wrapper STATIC_LIB IS_SIMPLE)

L4_link_libraries(PUBLIC libL4)

# In order to use IPlugin's vtable while using a static lib to split globals/statics
L4_use_pic()

# Report any undefined references
L4_report_undefined_refs()