include_guard(GLOBAL)

function(L4_set_cxx_standard)
    set_target_properties(${L4_CURRENT_PROJECT_SANITIZED_NAME} PROPERTIES CXX_STANDARD 23)
endfunction()

function(L4_force_pdbs)
    if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${L4_CURRENT_PROJECT_SANITIZED_NAME} PRIVATE /Zi)
        set_target_properties(${L4_CURRENT_PROJECT_SANITIZED_NAME} PROPERTIES
            LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
            COMPILE_PDB_NAME ${L4_CURRENT_PROJECT_SANITIZED_NAME}
            COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
        )
    endif()
endfunction()

function(L4_add_platform_macro)
    target_compile_definitions(${L4_CURRENT_PROJECT_SANITIZED_NAME} PRIVATE CONFIG_VERSION_PLATFORM=${PROJECT_PLATFORM} CONFIG_VERSION_PLATFORM_${PROJECT_PLATFORM})
endfunction()

function(L4_set_default_properties)
    L4_set_cxx_standard()
    L4_force_pdbs()
    L4_add_platform_macro()
endfunction()