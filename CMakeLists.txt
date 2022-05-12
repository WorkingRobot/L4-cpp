cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)


## Project Definition and Options ##

project(L4
    VERSION 0.1.0
    DESCRIPTION "The world's fastest game updater"
    HOMEPAGE_URL "https://epic.gl"
    LANGUAGES CXX)

# Get Git Info #
execute_process(COMMAND git -C ${CMAKE_SOURCE_DIR} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
    # No git info
    set(GIT_REV "ffffff")
    set(GIT_DIFF FALSE)
    set(GIT_BRANCH "orphaned")
else()
    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} diff --quiet --exit-code
        RESULT_VARIABLE GIT_DIFF)

    if (GIT_DIFF EQUAL 0)
        set(GIT_DIFF FALSE)
    else()
        set(GIT_DIFF TRUE)
    endif()

    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 6 GIT_REV)
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

set(PROJECT_VERSION_DIFF ${GIT_DIFF})
set(PROJECT_VERSION_HASH ${GIT_REV})
set(PROJECT_VERSION_BRANCH ${GIT_BRANCH})
set(PROJECT_VERSION_LONG ${PROJECT_NAME}/${PROJECT_VERSION})
if (NOT PROJECT_VERSION_BRANCH STREQUAL "main")
    set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}-${PROJECT_VERSION_BRANCH})
endif()
set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}+${PROJECT_VERSION_HASH})
if (PROJECT_VERSION_DIFF)
    set(PROJECT_VERSION_LONG ${PROJECT_VERSION_LONG}.dev)
endif()

function(ADD_VERSION_DEFS TARGET_FILE)
    set_property(
        SOURCE ${TARGET_FILE}
        APPEND
        PROPERTY COMPILE_DEFINITIONS
        CONFIG_PROJECT_NAME="${PROJECT_NAME}"
        CONFIG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        CONFIG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        CONFIG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        CONFIG_VERSION_HASH="${PROJECT_VERSION_HASH}"
        CONFIG_VERSION_BRANCH="${PROJECT_VERSION_BRANCH}"
        CONFIG_VERSION="${PROJECT_VERSION}"
        CONFIG_VERSION_LONG="${PROJECT_VERSION_LONG}"
    )
endfunction()

message(STATUS "Compiling for ${PROJECT_VERSION_LONG}")


## Source Files ##

function(CLEAN_FILE_PATH VARIABLE TARGET_FILE)
    get_filename_component(TARGET_FILE ${TARGET_FILE} ABSOLUTE)
    file(RELATIVE_PATH TARGET_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${TARGET_FILE})
    set(${VARIABLE} ${TARGET_FILE} PARENT_SCOPE)
endfunction()

function(ADD_FILE PROJECT TARGET_FILE)
    clean_file_path(TARGET_FILE ${TARGET_FILE})
    list(APPEND ${PROJECT}_SOURCES ${TARGET_FILE})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(ADD_DIR PROJECT TARGET_DIR)
    aux_source_directory(${TARGET_DIR} TARGET_DIR_SOURCES)
    foreach(TARGET_FILE ${TARGET_DIR_SOURCES})
        add_file(${PROJECT} ${TARGET_FILE})
    endforeach()
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(ADD_DIR_RECURSIVE PROJECT TARGET_DIR)
    file(GLOB_RECURSE CHILDREN ${TARGET_DIR}/*)
    foreach(CHILDPATH ${CHILDREN})
        get_filename_component(CHILD ${CHILDPATH} DIRECTORY)
        if(IS_DIRECTORY ${CHILD})
            list(APPEND DIRLIST ${CHILD})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES DIRLIST)

    foreach(DIRPATH ${DIRLIST})
        add_dir(${PROJECT} ${DIRPATH})
    endforeach()
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(REMOVE_FILE PROJECT TARGET_FILE)
    clean_file_path(TARGET_FILE ${TARGET_FILE})
    list(REMOVE_ITEM ${PROJECT}_SOURCES ${TARGET_FILE})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

set_property(GLOBAL PROPERTY ALL_SOURCES)
function(PUSH_SOURCES PROJECT)
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(SOURCE ${${PROJECT}_SOURCES})
        list(APPEND PROJECT_SOURCES ${REL_PATH}/${SOURCE})
    endforeach()
    set_property(GLOBAL APPEND PROPERTY ALL_SOURCES ${PROJECT_SOURCES})
endfunction()

function(FORCE_ENABLE_PDBS PROJECT)
    if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(${PROJECT} PRIVATE /Zi)
        set_target_properties(${PROJECT} PROPERTIES
            LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
            COMPILE_PDB_NAME ${PROJECT} 
            COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
        )
    endif()
endfunction()


## Add projects ##

add_subdirectory(src)
add_subdirectory(sources/riot)
add_subdirectory(deps/fastspd)


## Define extra targets ##

get_property(ALL_SOURCES GLOBAL PROPERTY ALL_SOURCES)
add_custom_target(
    format
    COMMAND clang-format -style=file -i ${ALL_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting all source files"
)
