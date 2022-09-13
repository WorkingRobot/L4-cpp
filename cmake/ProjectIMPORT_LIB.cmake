function(L4_define_project_internal)
    message(FATAL_ERROR "The import only library ${L4_CURRENT_PROJECT_NAME} cannot have any sources")
endfunction()

function(L4_define_project_alias_internal)

endfunction()

add_library(${L4_CURRENT_PROJECT_NAME} STATIC IMPORTED GLOBAL)
set_target_properties(${L4_CURRENT_PROJECT_NAME} PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${L4_CURRENT_PROJECT_IMPORTED_LOCATION}")
target_include_directories(${L4_CURRENT_PROJECT_NAME} INTERFACE .)