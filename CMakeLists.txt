cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)

include(FetchContent)
FetchContent_Declare(
  smake
  GIT_REPOSITORY https://github.com/WorkingRobot/smake.git
  GIT_TAG        10dab48e231689267310ca61f777450d169ebd04
)
FetchContent_MakeAvailable(smake)
#include(${smake_SOURCE_DIR}/smake.cmake)
include("J:/Code/Projects/smake/smake.cmake")

function(s_force_pdbs)
    if(MSVC)
        target_compile_options(${S_CURRENT_PROJECT_SANITIZED_NAME} PRIVATE /Zi)
        set_target_properties(${S_CURRENT_PROJECT_SANITIZED_NAME} PROPERTIES
            LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
            COMPILE_PDB_NAME ${S_CURRENT_PROJECT_SANITIZED_NAME}
            COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
        )
    endif()
endfunction()

## Project Definition ##

s_get_version_info(DEFAULT_BRANCH "main")
set(PROJECT_NAME "L4") # Change the casing because it looks cooler

project(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    DESCRIPTION ${PROJECT_DESCRIPTION}
    HOMEPAGE_URL "https://epic.gl"
    LANGUAGES C CXX)

message(STATUS "Compiling ${PROJECT_NAME} ${PROJECT_VERSION_LONG} for ${CMAKE_SYSTEM_NAME} (${VCPKG_TARGET_TRIPLET})")

## Set output directories ##

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Add projects ##

s_add_subdirectories_in(deps)
s_add_subdirectories_in(external)
s_add_subdirectories_in(modules)
s_add_subdirectories_in(plugins)
add_subdirectory(src)
