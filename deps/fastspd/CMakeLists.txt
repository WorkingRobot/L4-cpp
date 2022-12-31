s_project(L4::fastspd STATIC_LIB IS_SIMPLE INCLUDED_LOCATION .. PLATFORMS win)

s_link_libraries(PRIVATE setupapi rpcrt4)
