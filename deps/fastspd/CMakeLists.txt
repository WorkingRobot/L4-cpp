s_project(fastspd STATIC_LIB IS_SIMPLE PLATFORMS win)

s_link_libraries(PRIVATE setupapi rpcrt4)
