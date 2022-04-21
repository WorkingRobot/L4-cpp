cmake_minimum_required (VERSION 3.20)

## Project Definition and Options ##

## Source Files ##

add_dir_recursive(fastspd .)

## Define fastspd executable ##

add_library(fastspd STATIC ${fastspd_SOURCES})


## Dependencies ##

target_link_libraries(fastspd PRIVATE Setupapi Rpcrt4)

## Properties ##

# C++20 #
set_property(TARGET fastspd PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE MATCHES "Release")
   target_compile_options(fastspd PRIVATE /Zi)
   set_target_properties(fastspd PROPERTIES
        LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
        COMPILE_PDB_NAME fastspd 
        COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
   )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   #target_compile_options(fastspd PRIVATE /experimental:module /interface)
endif()