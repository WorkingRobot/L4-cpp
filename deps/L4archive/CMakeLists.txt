s_project(L4::archive HEADER_LIB IS_SIMPLE)

s_link_libraries(INTERFACE L4::mmio)
s_link_libraries(INTERFACE L4::utils)
