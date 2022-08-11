include_guard(GLOBAL)

function(L4_sanitize_project_name VARIABLE NAME)
    string(REPLACE "::" "-" NAME ${NAME})
    set(${VARIABLE} ${NAME} PARENT_SCOPE)
endfunction()

function(L4_project NAME TYPE)
    set(SUPPORTED_TYPES "SHARED_LIB;STATIC_LIB;HEADER_LIB;MODULE_LIB;EXEC_NOGUI;EXEC_WIN32")
    if (NOT ${TYPE} IN_LIST SUPPORTED_TYPES)
        message(FATAL_ERROR "Invalid project type for ${L4_CURRENT_PROJECT_NAME}: ${TYPE}")
    endif()

    if (TYPE STREQUAL "HEADER_LIB")
        if (NOT L4_CURRENT_PROJECT_IS_SIMPLE)
            message(FATAL_ERROR "The header only library ${L4_CURRENT_PROJECT_NAME} must be defined with L4_simple_project")
        endif()
    else()
        set(L4_CURRENT_PROJECT_IS_SIMPLE FALSE PARENT_SCOPE)
    endif()

    L4_sanitize_project_name(SANITIZED_NAME ${NAME})

    set(L4_CURRENT_PROJECT_NAME ${NAME} PARENT_SCOPE)
    set(L4_CURRENT_PROJECT_SANITIZED_NAME ${SANITIZED_NAME} PARENT_SCOPE)
    set(L4_CURRENT_PROJECT_TYPE ${TYPE} PARENT_SCOPE)
endfunction()

function(L4_define_project_alias)
    if (L4_CURRENT_PROJECT_NAME STREQUAL L4_CURRENT_PROJECT_SANITIZED_NAME)
        return()
    endif()

    set(LIBRARY_TYPES "SHARED_LIB;STATIC_LIB;HEADER_LIB;MODULE_LIB")
    set(EXECUTABLE_TYPES "EXEC_NOGUI;EXEC_WIN32")
    if (L4_CURRENT_PROJECT_TYPE IN_LIST LIBRARY_TYPES)
        add_library(${L4_CURRENT_PROJECT_NAME} ALIAS ${L4_CURRENT_PROJECT_SANITIZED_NAME})
    elseif(L4_CURRENT_PROJECT_TYPE IN_LIST EXECUTABLE_TYPES)
        add_executable(${L4_CURRENT_PROJECT_NAME} ALIAS ${L4_CURRENT_PROJECT_SANITIZED_NAME})
    endif()
endfunction()

function(L4_define_project)
    if (L4_CURRENT_PROJECT_TYPE STREQUAL "SHARED_LIB")
        add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} SHARED ${L4_CURRENT_PROJECT_SOURCES})
        target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE .)
    elseif(L4_CURRENT_PROJECT_TYPE STREQUAL "STATIC_LIB")
        add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} STATIC ${L4_CURRENT_PROJECT_SOURCES})
        target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE .)
    elseif(L4_CURRENT_PROJECT_TYPE STREQUAL "HEADER_LIB")
        add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE)
        target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} INTERFACE .)
    elseif(L4_CURRENT_PROJECT_TYPE STREQUAL "MODULE_LIB")
        add_library(${L4_CURRENT_PROJECT_SANITIZED_NAME} MODULE ${L4_CURRENT_PROJECT_SOURCES})
    elseif(L4_CURRENT_PROJECT_TYPE STREQUAL "EXEC_NOGUI")
        add_executable(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${L4_CURRENT_PROJECT_SOURCES})
    elseif(L4_CURRENT_PROJECT_TYPE STREQUAL "EXEC_WIN32")
        add_executable(${L4_CURRENT_PROJECT_SANITIZED_NAME} WIN32 ${L4_CURRENT_PROJECT_SOURCES})
    else()
        message(FATAL_ERROR "Invalid project type for ${L4_CURRENT_PROJECT_NAME}: ${L4_CURRENT_PROJECT_TYPE}")
    endif()

    L4_define_project_alias()

    if (NOT L4_CURRENT_PROJECT_TYPE STREQUAL "HEADER_LIB")
        L4_set_default_properties()
    endif()
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
    if(L4_CURRENT_PROJECT_TYPE STREQUAL "HEADER_LIB" AND L4_CURRENT_PROJECT_SOURCES)
        message(FATAL_ERROR "The header only library ${L4_CURRENT_PROJECT_NAME} cannot have explicitly defined sources")
    endif()
    L4_define_project()
    L4_push_sources()
endfunction()

macro(L4_hoist_variable VARIABLE)
    set(${VARIABLE} ${${VARIABLE}} PARENT_SCOPE)
endmacro()

function(L4_project_simple NAME TYPE)
    set(L4_CURRENT_PROJECT_IS_SIMPLE TRUE)
    L4_project(${NAME} ${TYPE})

    if(NOT L4_CURRENT_PROJECT_TYPE STREQUAL "HEADER_LIB")
        L4_add_dir(. RECURSIVE)
    endif()

    L4_end_sources()

    L4_hoist_variable(L4_CURRENT_PROJECT_NAME)
    L4_hoist_variable(L4_CURRENT_PROJECT_SANITIZED_NAME)
    L4_hoist_variable(L4_CURRENT_PROJECT_TYPE)
    L4_hoist_variable(L4_CURRENT_PROJECT_IS_SIMPLE)
endfunction()

function(L4_project_imported NAME IMPORTED_LOCATION)
    add_library(${NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${NAME} PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${IMPORTED_LOCATION}")
    target_include_directories(${NAME} INTERFACE .)
endfunction()

function(L4_clean_file_path VARIABLE PATH)
    get_filename_component(PATH ${PATH} ABSOLUTE)
    file(RELATIVE_PATH ${VARIABLE} ${CMAKE_CURRENT_SOURCE_DIR} ${PATH})
    L4_hoist_variable(${VARIABLE})
endfunction()

function(L4_add_file PATH)
    L4_clean_file_path(PATH ${PATH})
    list(APPEND L4_CURRENT_PROJECT_SOURCES ${PATH})
    L4_hoist_variable(L4_CURRENT_PROJECT_SOURCES)
endfunction()

function(L4_del_file PATH)
    L4_clean_file_path(PATH ${PATH})
    list(REMOVE_ITEM L4_CURRENT_PROJECT_SOURCES ${PATH})
    L4_hoist_variable(L4_CURRENT_PROJECT_SOURCES)
endfunction()

function(L4_add_dir_non_recursive PATH)
    aux_source_directory(${PATH} TARGET_DIR_SOURCES)
    foreach(TARGET_FILE ${TARGET_DIR_SOURCES})
        L4_add_file(${TARGET_FILE})
    endforeach()
    L4_hoist_variable(L4_CURRENT_PROJECT_SOURCES)
endfunction()

function(L4_add_dir_recursive PATH)
    file(GLOB_RECURSE CHILD_LIST ${PATH}/*)
    foreach(CHILD_PATH ${CHILD_LIST})
        get_filename_component(DIR_PATH ${CHILD_PATH} DIRECTORY)
        if(IS_DIRECTORY ${DIR_PATH})
            list(APPEND DIR_LIST ${DIR_PATH})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES DIR_LIST)

    foreach(DIR_PATH ${DIR_LIST})
        L4_add_dir_non_recursive(${DIR_PATH})
    endforeach()
    L4_hoist_variable(L4_CURRENT_PROJECT_SOURCES)
endfunction()

function(L4_add_dir PATH)
    cmake_parse_arguments(PARSE_ARGV 1 DIR "RECURSIVE" "" "")
    if (${DIR_RECURSIVE})
        L4_add_dir_recursive(${PATH})
    else()
        L4_add_dir_non_recursive(${PATH})
    endif()
    L4_hoist_variable(L4_CURRENT_PROJECT_SOURCES)
endfunction()

function(L4_add_subdirectories_in PATH)
    file(GLOB CHILD_LIST ${PATH}/*)
    foreach(CHILD_PATH ${CHILD_LIST})
        if(IS_DIRECTORY ${CHILD_PATH})
            list(APPEND DIR_LIST ${CHILD_PATH})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES DIR_LIST)

    foreach(DIR_PATH ${DIR_LIST})
        add_subdirectory(${DIR_PATH})
    endforeach()
endfunction()

function(L4_get_git_info)
    execute_process(COMMAND git -C ${CMAKE_SOURCE_DIR} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REVISION
                ERROR_QUIET)

    if (GIT_REVISION STREQUAL "")
        # No git info
        set(GIT_BRANCH "orphaned")
        set(GIT_REVISION "ffffff")
        set(GIT_IS_MODIFIED FALSE)
    else()
        string(STRIP "${GIT_REVISION}" GIT_REVISION)
        string(SUBSTRING "${GIT_REVISION}" 1 6 GIT_REVISION)

        execute_process(
            COMMAND git -C ${CMAKE_SOURCE_DIR} diff --quiet --exit-code
            RESULT_VARIABLE GIT_IS_MODIFIED)
        if (GIT_IS_MODIFIED EQUAL 0)
            set(GIT_IS_MODIFIED FALSE)
        else()
            set(GIT_IS_MODIFIED TRUE)
        endif()

        execute_process(
            COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
            OUTPUT_VARIABLE GIT_BRANCH)
        string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
    endif()

    L4_hoist_variable(GIT_BRANCH)
    L4_hoist_variable(GIT_REVISION)
    L4_hoist_variable(GIT_IS_MODIFIED)
endfunction()

function(L4_retrieve_version_info)
    L4_get_git_info()

    set(PROJECT_VERSION_BRANCH ${GIT_BRANCH})
    set(PROJECT_VERSION_REVISION ${GIT_REVISION})
    set(PROJECT_VERSION_IS_MODIFIED ${GIT_IS_MODIFIED})

    set(PROJECT_VERSION_LONG ${PROJECT_VERSION})
    if (NOT PROJECT_VERSION_BRANCH STREQUAL "main")
        set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}-${PROJECT_VERSION_BRANCH})
    endif()
    set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}+${PROJECT_VERSION_REVISION})
    if (PROJECT_VERSION_IS_MODIFIED)
        set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}.dev)
    endif()

    L4_hoist_variable(PROJECT_VERSION_BRANCH)
    L4_hoist_variable(PROJECT_VERSION_REVISION)
    L4_hoist_variable(PROJECT_VERSION_IS_MODIFIED)
    L4_hoist_variable(PROJECT_VERSION_LONG)
endfunction()

function(L4_add_version_defs PATH)
    set_property(
        SOURCE ${PATH}
        APPEND
        PROPERTY COMPILE_DEFINITIONS
        CONFIG_PROJECT_NAME="${PROJECT_NAME}"
        CONFIG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        CONFIG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        CONFIG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        CONFIG_VERSION_BRANCH="${PROJECT_VERSION_BRANCH}"
        CONFIG_VERSION_REVISION="${PROJECT_VERSION_REVISION}"
        $<$<BOOL:${PROJECT_VERSION_IS_MODIFIED}>:CONFIG_VERSION_IS_MODIFIED>
        CONFIG_VERSION="${PROJECT_VERSION}"
        CONFIG_VERSION_LONG="${PROJECT_VERSION_LONG}"
    )
endfunction()

function(L4_set_cxx_standard)
    set_property(TARGET ${L4_CURRENT_PROJECT_SANITIZED_NAME} PROPERTY CXX_STANDARD 23)
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

function(L4_set_default_properties)
    L4_set_cxx_standard()
    L4_force_pdbs()
endfunction()

function(L4_add_dependency NAME VISIBILITY)
    cmake_language(CALL L4_dependency_${NAME} ${VISIBILITY})
endfunction()

function(L4_link_libraries)
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${ARGV})
endfunction()

function(L4_compile_definitions)
    target_compile_definitions(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${ARGV})
endfunction()

include(CMakeL4Deps.cmake)