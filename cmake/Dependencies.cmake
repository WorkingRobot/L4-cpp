include_guard(GLOBAL)

function(L4_add_dependency_pkgconfig NAME)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PKGCONFIG_LIB REQUIRED ${NAME})
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} ${PKGCONFIG_LIB_LIBRARIES})
    target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} ${PKGCONFIG_LIB_INCLUDE_DIRS})
endfunction()

function(L4_add_dependency_manual INCLUDE_PATH LIBRARY_NAME)
    find_path(MANUAL_INCLUDE_DIR ${INCLUDE_PATH} REQUIRED)
    find_library(MANUAL_LIBRARY ${LIBRARY_NAME} REQUIRED)
    target_include_directories(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} ${MANUAL_INCLUDE_DIR})
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} ${MANUAL_LIBRARY})
endfunction()

function(L4_add_dependency_findpkg PKG_NAME)
    cmake_parse_arguments(PARSE_ARGV 1 FINDPKG "CONFIG" "" "LIBS")
    if (${FINDPKG_CONFIG})
        find_package(${PKG_NAME} CONFIG REQUIRED)
    else()
        find_package(${PKG_NAME} REQUIRED)
    endif()
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} ${FINDPKG_LIBS})
endfunction()

function(L4_add_dependency NAME VISIBILITY)
    cmake_language(CALL L4_dependency_${NAME})
endfunction()

function(L4_dependency_JSON)
    L4_add_dependency_findpkg(RapidJSON CONFIG LIBS rapidjson)
endfunction()

function(L4_dependency_HTTP)
    L4_add_dependency_findpkg(cpr CONFIG LIBS cpr::cpr)
endfunction()

function(L4_dependency_libbacktrace)
    L4_add_dependency_manual(backtrace.h backtrace)
endfunction()

function(L4_dependency_breakpad)
    L4_add_dependency_findpkg(unofficial-breakpad CONFIG LIBS unofficial::breakpad::libbreakpad unofficial::breakpad::libbreakpad_client)
endfunction()

function(L4_dependency_libfmt)
    L4_add_dependency_findpkg(fmt CONFIG LIBS fmt::fmt)
endfunction()

function(L4_dependency_date)
    L4_add_dependency_findpkg(date CONFIG LIBS date::date date::date-tz)
endfunction()

function(L4_dependency_OpenSSL)
    L4_add_dependency_findpkg(OpenSSL LIBS OpenSSL::Crypto)
endfunction()