L4_project(L4::source::wrapper STATIC_LIB IS_SIMPLE)

L4_link_libraries(PUBLIC libL4)

# In order to use IPlugin's vtable while using a static lib to split globals/statics
L4_use_pic()