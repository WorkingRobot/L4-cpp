function(L4_define_project_internal)
    add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} MODULE ${L4_CURRENT_PROJECT_SOURCES})

    L4_set_default_properties()
endfunction()

function(L4_define_project_alias_internal)
    add_library(${L4_CURRENT_PROJECT_NAME} ALIAS ${L4_CURRENT_PROJECT_SANITIZED_NAME})
endfunction()

if (L4_CURRENT_PROJECT_IS_SIMPLE)
    L4_add_dir(. RECURSIVE)
    L4_end_sources()
endif()