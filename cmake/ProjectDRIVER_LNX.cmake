function(L4_define_project_internal)
    foreach(PROJECT_SOURCE ${L4_CURRENT_PROJECT_SOURCES})
        get_filename_component(PROJECT_SOURCE_EXT ${PROJECT_SOURCE} LAST_EXT)
        if (NOT PROJECT_SOURCE_EXT STREQUAL ".c")
            message(FATAL_ERROR "The Linux kernel driver ${L4_CURRENT_PROJECT_NAME} only supports .c files")
        endif()
    endforeach()
    
    string(REPLACE ";" " " KBUILD_PROJECT_SOURCES "${L4_CURRENT_PROJECT_SOURCES}")
    set(KBUILD_PROJECT_NAME "${L4_CURRENT_PROJECT_SANITIZED_NAME}")
    configure_file(Kbuild.in Kbuild @ONLY)

    add_custom_command(OUTPUT ${L4_CURRENT_PROJECT_SOURCES}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko files"
        VERBATIM)

    L4_get_kernel_dir(KERNEL_DIR)
    set(KBUILD_CMD make -C ${KERNEL_DIR} M=${CMAKE_CURRENT_BINARY_DIR})
    add_custom_command(OUTPUT ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko
        COMMAND ${KBUILD_CMD} modules
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${L4_CURRENT_PROJECT_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
        COMMENT "Building ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko"
        VERBATIM)
    add_custom_target(${L4_CURRENT_PROJECT_SANITIZED_NAME} ALL DEPENDS ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko)

    if (DEFINED ENV{KBUILD_MOK_PATH})
        L4_get_driver_sign_command(SIGN_CMD)
        add_custom_command(TARGET ${L4_CURRENT_PROJECT_SANITIZED_NAME} POST_BUILD
            COMMAND ${SIGN_CMD} ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Signing ${L4_CURRENT_PROJECT_SANITIZED_NAME}.ko"
            VERBATIM)
    endif()

    add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME}-dummy MODULE ${L4_CURRENT_PROJECT_SOURCES})
    set_target_properties(${L4_CURRENT_PROJECT_SANITIZED_NAME}-dummy PROPERTIES EXCLUDE_FROM_ALL TRUE)
    target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME}-dummy PUBLIC "${KERNEL_DIR}/include" "${KERNEL_DIR}/arch/x86/include" "${KERNEL_DIR}/arch/x86/include/generated")
    target_compile_definitions(${L4_CURRENT_PROJECT_SANITIZED_NAME}-dummy PRIVATE -D__KERNEL__ -DMODULE)
endfunction()

function(L4_define_project_alias_internal)
    add_library(${L4_CURRENT_PROJECT_NAME}-dummy ALIAS ${L4_CURRENT_PROJECT_SANITIZED_NAME}-dummy)
endfunction()

if (L4_CURRENT_PROJECT_IS_SIMPLE)
    L4_add_dir(. RECURSIVE)
    L4_end_sources()
endif()