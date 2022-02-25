cmake_minimum_required (VERSION 3.20)

## Project Definition and Options ##

## Source Files ##

add_dir_recursive(L4 .)
ADD_VERSION_DEFS("utils/Version.cpp")

## Define L4 executable ##

add_executable(L4 ${L4_SOURCES})


## Dependencies ##

find_package(PkgConfig REQUIRED)

# GUI #

if (FALSE)
find_library(GTK_LIB gtk-4 REQUIRED)
get_filename_component(LIB_PATH ${GTK_LIB} DIRECTORY)
target_link_directories(L4 PRIVATE ${LIB_PATH})

pkg_check_modules(GTK REQUIRED gtkmm-4.0)
target_link_libraries(L4 ${GTK_LIBRARIES})
target_include_directories(L4 PUBLIC ${GTK_INCLUDE_DIRS})
target_compile_options(L4 PUBLIC ${GTK_CFLAGS_OTHER})
endif()

# Json #
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(L4 PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

# Http #
find_package(cpr CONFIG REQUIRED)
target_link_libraries(L4 PRIVATE cpr::cpr)

# Zstd #
find_package(zstd CONFIG REQUIRED)
target_link_libraries(L4 PRIVATE zstd::libzstd_shared)

# Ntdll #
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/ntdll)
target_link_libraries(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/ntdll/ntdll64.lib)

# WinSpd #
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/winspd)
target_link_libraries(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/winspd/winspd-x64.lib)


## Properties ##

# C++20 #
set_property(TARGET L4 PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE MATCHES "Release")
   target_compile_options(L4 PRIVATE /Zi)
   set_target_properties(L4 PROPERTIES
        LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
        COMPILE_PDB_NAME L4 
        COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
   )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
   #target_compile_options(L4 PRIVATE /experimental:module /interface)
endif()