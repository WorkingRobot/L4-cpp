## Source Files ##

add_file(L4 Main.cpp)
add_file(L4 RamDisk.cpp)
add_file(L4 BasicDisk.cpp)
add_dir(L4 disk)
add_dir(L4 utils)
add_dir(L4 archive)
add_dir(L4 mmio)
push_sources(L4)


## Define L4 executable ##

add_executable(L4 ${L4_SOURCES})


## Dependencies ##

find_package(PkgConfig REQUIRED)

if (FALSE)
# GUI #

#find_library(GTK_LIB gtk-4 REQUIRED)
#get_filename_component(LIB_PATH ${GTK_LIB} DIRECTORY)
#target_link_directories(L4 PRIVATE ${LIB_PATH})

pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
target_link_directories(L4 PRIVATE ${GTKMM_LIBRARY_DIRS})
target_link_libraries(L4 PRIVATE ${GTKMM_LIBRARIES})
target_include_directories(L4 PRIVATE ${GTKMM_INCLUDE_DIRS})
target_compile_options(L4 PRIVATE -Wno-ignored-attributes)
endif()

if (FALSE)
# Zstd #
find_package(zstd CONFIG REQUIRED)
target_link_libraries(L4 PRIVATE zstd::libzstd_shared)
endif()

# Ntdll #
target_link_libraries(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/ntdll/ntdll64.lib)
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/ntdll)

# WinSpd Wrapper #
target_link_libraries(L4 PRIVATE fastspd)
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/fastspd)

# Web #
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/L4web)
target_link_libraries(L4 PRIVATE L4web)

# Discord Rich Presence #
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR}/deps/L4discord)
target_link_libraries(L4 PRIVATE L4discord)


## Properties ##

# C++20 #
set_property(TARGET L4 PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(L4)
