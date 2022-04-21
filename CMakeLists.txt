cmake_minimum_required (VERSION 3.20)
cmake_policy(SET CMP0091 NEW)

## Project Definition and Options ##

project(L4
    VERSION 3.90.0
    DESCRIPTION "The world's fastest game updater"
    HOMEPAGE_URL "https://epic.gl")

# Get Git Info #
execute_process(COMMAND git -C ${CMAKE_SOURCE_DIR} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
    # No git info
    set(GIT_REV "fffffff")
    set(GIT_DIFF "")
    set(GIT_BRANCH "orphaned")
else()
    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} diff --quiet --exit-code
        RESULT_VARIABLE GIT_DIFF)
    if (GIT_DIFF EQUAL "0")
        set(GIT_DIFF "")
    else()
        set(GIT_DIFF "+")
    endif()
    execute_process(
        COMMAND git -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 6 GIT_REV)
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

set(PROJECT_VERSION_HASH ${GIT_REV}${GIT_DIFF})
set(PROJECT_VERSION_BRANCH ${GIT_BRANCH})
set(PROJECT_VERSION_LONG ${PROJECT_VERSION}-${PROJECT_VERSION_HASH}++${PROJECT_NAME}+${PROJECT_VERSION_BRANCH})

message(STATUS "Compiling for ${PROJECT_VERSION_LONG}")

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


## Source Files ##

function(ADD_DIR PROJECT TARGET_DIR)
    aux_source_directory(${TARGET_DIR} TARGET_DIR_SOURCES)
    list(APPEND ${PROJECT}_SOURCES ${TARGET_DIR_SOURCES})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(ADD_FILE PROJECT TARGET_FILE)
    list(APPEND ${PROJECT}_SOURCES ${TARGET_FILE})
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
    list(REMOVE_ITEM ${PROJECT}_SOURCES ${TARGET_FILE})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()


## Add projects ##

add_subdirectory(src)
add_subdirectory(deps/fastspd)

## Define install targets ##

if (CMAKE_BUILD_TYPE MATCHES "Release")
    file(GLOB GTK_DLLS
        "${CMAKE_BINARY_DIR}/src/brotlicommon.dll"
        "${CMAKE_BINARY_DIR}/src/brotlidec.dll"
        "${CMAKE_BINARY_DIR}/src/bz2.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-2.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-gobject-2.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-script-interpreter-2.dll"
        "${CMAKE_BINARY_DIR}/src/epoxy-0.dll"
        "${CMAKE_BINARY_DIR}/src/fontconfig-1.dll"
        "${CMAKE_BINARY_DIR}/src/freetype.dll"
        "${CMAKE_BINARY_DIR}/src/fribidi-0.dll"
        "${CMAKE_BINARY_DIR}/src/gdk_pixbuf-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gio-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/glib-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gmodule-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gobject-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/graphene-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gtk-4-1.dll"
        "${CMAKE_BINARY_DIR}/src/harfbuzz.dll"
        "${CMAKE_BINARY_DIR}/src/iconv-2.dll"
        "${CMAKE_BINARY_DIR}/src/intl-8.dll"
        "${CMAKE_BINARY_DIR}/src/jpeg62.dll"
        "${CMAKE_BINARY_DIR}/src/libexpat.dll"
        "${CMAKE_BINARY_DIR}/src/libffi.dll"
        "${CMAKE_BINARY_DIR}/src/libpng16.dll"
        "${CMAKE_BINARY_DIR}/src/lzma.dll"
        "${CMAKE_BINARY_DIR}/src/lzo2.dll"
        "${CMAKE_BINARY_DIR}/src/pango-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangocairo-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangoft2-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangowin32-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pcre.dll"
        "${CMAKE_BINARY_DIR}/src/tiff.dll"
        "${CMAKE_BINARY_DIR}/src/zlib1.dll")
    file(GLOB GTKMM_DLLS
        "${CMAKE_BINARY_DIR}/src/cairomm-1.16-1.dll"
        "${CMAKE_BINARY_DIR}/src/giomm-2.68-1.dll"
        "${CMAKE_BINARY_DIR}/src/glibmm-2.68-1.dll"
        "${CMAKE_BINARY_DIR}/src/gtkmm-4.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangomm-2.48-1.dll"
        "${CMAKE_BINARY_DIR}/src/sigc-3.0.dll")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    file(GLOB GTK_DLLS
        "${CMAKE_BINARY_DIR}/src/brotlicommon.dll"
        "${CMAKE_BINARY_DIR}/src/brotlidec.dll"
        "${CMAKE_BINARY_DIR}/src/bz2d.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-2.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-gobject-2.dll"
        "${CMAKE_BINARY_DIR}/src/cairo-script-interpreter-2.dll"
        "${CMAKE_BINARY_DIR}/src/epoxy-0.dll"
        "${CMAKE_BINARY_DIR}/src/fontconfig-1.dll"
        "${CMAKE_BINARY_DIR}/src/freetyped.dll"
        "${CMAKE_BINARY_DIR}/src/fribidi-0.dll"
        "${CMAKE_BINARY_DIR}/src/gdk_pixbuf-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gio-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/glib-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gmodule-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gobject-2.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/graphene-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/gtk-4-1.dll"
        "${CMAKE_BINARY_DIR}/src/harfbuzz.dll"
        "${CMAKE_BINARY_DIR}/src/iconv-2.dll"
        "${CMAKE_BINARY_DIR}/src/intl-8.dll"
        "${CMAKE_BINARY_DIR}/src/jpeg62.dll"
        "${CMAKE_BINARY_DIR}/src/libexpatd.dll"
        "${CMAKE_BINARY_DIR}/src/libffi.dll"
        "${CMAKE_BINARY_DIR}/src/libpng16d.dll"
        "${CMAKE_BINARY_DIR}/src/lzmad.dll"
        "${CMAKE_BINARY_DIR}/src/lzo2.dll"
        "${CMAKE_BINARY_DIR}/src/pango-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangocairo-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangoft2-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangowin32-1.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pcred.dll"
        "${CMAKE_BINARY_DIR}/src/tiffd.dll"
        "${CMAKE_BINARY_DIR}/src/zlibd1.dll")
    file(GLOB GTKMM_DLLS
        "${CMAKE_BINARY_DIR}/src/cairomm-1.16-1.dll"
        "${CMAKE_BINARY_DIR}/src/giomm-2.68-1.dll"
        "${CMAKE_BINARY_DIR}/src/glibmm-2.68-1.dll"
        "${CMAKE_BINARY_DIR}/src/gtkmm-4.0-0.dll"
        "${CMAKE_BINARY_DIR}/src/pangomm-2.48-1.dll"
        "${CMAKE_BINARY_DIR}/src/sigc-3.0.dll")
else()
    message(FATAL_ERROR "Invalid build type ${CMAKE_BUILD_TYPE}")
endif()

if (FALSE)
install(TARGETS L4 DESTINATION ".")
install(PROGRAMS ${GTK_DLLS} ${GTKMM_DLLS} DESTINATION ".")
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
include(InstallRequiredSystemLibraries)

# CPack #

set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_VENDOR "WorkingRobot")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_WIX_UPGRADE_GUID "687AD339-861D-47E8-B80F-BC5D1FF90A1A")
set(CPACK_WIX_PROPERTY_ARPHELPLINK "https://epic.gl/discord")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://epic.gl")
set(CPACK_WIX_PROPERTY_ARPURLUPDATEINFO "https://epic.gl/changelog")

include(CPack)
endif()
