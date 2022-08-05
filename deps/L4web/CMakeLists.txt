## Source Files ##

add_dir_recursive(L4web .)
push_sources(L4web)

## Define fastspd executable ##

add_library(L4web STATIC ${L4web_SOURCES})


## Dependencies ##

# Json #
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(L4web PUBLIC ${RAPIDJSON_INCLUDE_DIRS})

# Http #
find_package(cpr CONFIG REQUIRED)
target_link_libraries(L4web PRIVATE cpr::cpr)


## Properties ##

# C++20 #
set_property(TARGET L4web PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(L4web)
