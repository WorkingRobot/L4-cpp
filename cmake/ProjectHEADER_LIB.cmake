function(L4_define_project_internal)
    if(L4_CURRENT_PROJECT_SOURCES)
        message(FATAL_ERROR "The header only library ${L4_CURRENT_PROJECT_NAME} cannot have explicitly defined sources")
    endif()

    add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE)
    target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE .)
endfunction()

function(L4_define_project_alias_internal)
    add_library(${L4_CURRENT_PROJECT_NAME} ALIAS ${L4_CURRENT_PROJECT_SANITIZED_NAME})
endfunction()

L4_end_sources()