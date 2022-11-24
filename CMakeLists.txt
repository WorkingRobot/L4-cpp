cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)

include(FetchContent)
FetchContent_Declare(
  smake
  GIT_REPOSITORY https://github.com/WorkingRobot/smake.git
  GIT_TAG        v2
)
FetchContent_MakeAvailable(smake)
include(${smake_SOURCE_DIR}/smake.cmake)

## Project Definition ##

s_get_version_info(DEFAULT_BRANCH "main")
project(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    DESCRIPTION ${PROJECT_DESCRIPTION}
    HOMEPAGE_URL "https://epic.gl"
    LANGUAGES CXX)

message(STATUS "Compiling ${PROJECT_NAME} ${PROJECT_VERSION_LONG} for ${CMAKE_SYSTEM_NAME} (${VCPKG_TARGET_TRIPLET})")

## Set output directories ##

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Add projects ##

s_add_subdirectories_in(deps)
s_add_subdirectories_in(plugins)
# s_add_subdirectories_in(driver)
add_subdirectory(src)
