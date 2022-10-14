L4_project(L4 EXEC_NOGUI IS_DISABLED)

L4_add_file(MainDiscord.cpp)

L4_end_sources()

if (FALSE)
find_package(PkgConfig REQUIRED)

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

#L4_link_libraries(PRIVATE fastspd)
L4_link_libraries(PRIVATE L4::web)
L4_link_libraries(PRIVATE L4::discord)
L4_link_libraries(PRIVATE L4::sourcemgr)
L4_link_libraries(PRIVATE L4::log)
L4_link_libraries(PRIVATE L4::utils)
# L4_link_libraries(PRIVATE ntdll)
