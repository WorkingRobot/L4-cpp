s_project(L4::resources STATIC_LIB INCLUDED_LOCATION ..)

function(add_gresource_xml_file GRESOURCE_XML)
    find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

    get_filename_component(GRESOURCE_DIRECTORY ${GRESOURCE_XML} DIRECTORY)

    file(GLOB_RECURSE GRESOURCE_DEPS ${GRESOURCE_DIRECTORY}/*)
    list(REMOVE_ITEM GRESOURCE_DEPS ${GRESOURCE_XML})

    get_filename_component(GRESOURCE_C ${GRESOURCE_XML} NAME_WE)
    set(GRESOURCE_C ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}.gresource.c)

    add_custom_command(
        OUTPUT ${GRESOURCE_C}
        COMMAND
            ${GLIB_COMPILE_RESOURCES}
            --generate
            --target=${GRESOURCE_C}
            ${GRESOURCE_XML}
        MAIN_DEPENDENCY ${GRESOURCE_XML} 
        DEPENDS ${GRESOURCE_DEPS}
        WORKING_DIRECTORY ${GRESOURCE_DIRECTORY}
        VERBATIM
    )

    s_add_file_force(${GRESOURCE_C})
    s_hoist_variable(S_CURRENT_PROJECT_SOURCES)
endfunction()

s_add_dir_recursive(.)
add_gresource_xml_file(${CMAKE_SOURCE_DIR}/resources/L4.gresource.xml)
s_end_sources()

s_add_dependency(giomm PUBLIC)
