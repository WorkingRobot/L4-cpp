cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)
include(cmake/L4.cmake)

## Project Definition ##

project(L4
    VERSION 0.1.0
    DESCRIPTION "The world's fastest game updater"
    HOMEPAGE_URL "https://epic.gl"
    LANGUAGES CXX C)

L4_retrieve_version_info()

message(STATUS "Compiling ${PROJECT_NAME} ${PROJECT_VERSION_LONG} for ${CMAKE_SYSTEM_NAME} (${VCPKG_TARGET_TRIPLET})")

## Set output directories ##

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Add projects ##

L4_add_subdirectories_in(deps)
L4_add_subdirectories_in(sources)
L4_add_subdirectories_in(driver)
add_subdirectory(src)

## Define extra targets ##

get_property(ALL_SOURCES GLOBAL PROPERTY ALL_SOURCES)
add_custom_target(
    format
    COMMAND clang-format -style=file -i ${ALL_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting all source files"
)
