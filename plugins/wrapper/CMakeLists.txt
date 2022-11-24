s_project(L4::plugin::wrapper STATIC_LIB IS_SIMPLE)

s_link_libraries(PUBLIC libL4)

# In order to use IPlugin's vtable while using a static lib to split globals/statics
s_use_pic()

# Report any undefined references
s_report_undefined_refs()