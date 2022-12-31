s_project(ext::ntdll IMPORT_LIB PLATFORMS win)

set_target_properties(ext::ntdll PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/ntdll64.lib")