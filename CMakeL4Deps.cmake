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
