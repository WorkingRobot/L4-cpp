include_guard(GLOBAL)

function(L4_sanitize_project_name VARIABLE NAME)
    string(REPLACE "::" "-" NAME ${NAME})
    set(${VARIABLE} ${NAME} PARENT_SCOPE)
endfunction()

function(L4_project_internal)
    L4_sanitize_project_name(L4_CURRENT_PROJECT_SANITIZED_NAME ${L4_CURRENT_PROJECT_NAME})
    L4_hoist_variable(L4_CURRENT_PROJECT_SANITIZED_NAME)

    include(Project${L4_CURRENT_PROJECT_TYPE})
endfunction()

macro(L4_project NAME TYPE)
    set(L4_CURRENT_PROJECT_NAME ${NAME})
    set(L4_CURRENT_PROJECT_TYPE ${TYPE})

    cmake_parse_arguments(L4_CURRENT_PROJECT "IS_SIMPLE;IS_DISABLED;IS_SIGNED" "IMPORTED_LOCATION" "PLATFORMS;DRIVER_STATIC_LIBS" ${ARGN})

    if (L4_CURRENT_PROJECT_IS_DISABLED)
        message("Not building ${L4_CURRENT_PROJECT_NAME} because it's disabled")
        return()
    endif()

    if (L4_CURRENT_PROJECT_PLATFORMS)
        if (NOT PROJECT_PLATFORM IN_LIST L4_CURRENT_PROJECT_PLATFORMS)
            message("Not building ${L4_CURRENT_PROJECT_NAME} because it's unsupported (only for ${L4_CURRENT_PROJECT_PLATFORMS})")
            return()
        endif()
    endif()

    L4_project_internal()
endmacro()

function(L4_define_project_alias)
    if (L4_CURRENT_PROJECT_NAME STREQUAL L4_CURRENT_PROJECT_SANITIZED_NAME)
        return()
    endif()

    L4_define_project_alias_internal()
endfunction()

function(L4_define_project)
    L4_define_project_internal()

    L4_define_project_alias()
endfunction()

set_property(GLOBAL PROPERTY L4_ALL_SOURCES)
function(L4_push_sources)
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(SOURCE ${L4_CURRENT_PROJECT_SOURCES})
        list(APPEND PROJECT_SOURCES ${REL_PATH}/${SOURCE})
    endforeach()
    set_property(GLOBAL APPEND PROPERTY L4_ALL_SOURCES ${PROJECT_SOURCES})
endfunction()

function(L4_end_sources)
    L4_define_project()
    L4_push_sources()
endfunction()

function(L4_link_libraries)
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${ARGV})
endfunction()

function(L4_compile_definitions)
    target_compile_definitions(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${ARGV})
endfunction()