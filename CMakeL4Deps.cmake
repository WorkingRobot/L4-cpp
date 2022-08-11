function(L4_dependency_JSON VISIBILITY)
    find_package(RapidJSON CONFIG REQUIRED)
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} rapidjson)
endfunction()

function(L4_dependency_HTTP VISIBILITY)
    find_package(cpr CONFIG REQUIRED)
    target_link_libraries(${L4_CURRENT_PROJECT_SANITIZED_NAME} ${VISIBILITY} cpr::cpr)
endfunction()