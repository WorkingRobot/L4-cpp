## Source Files ##

add_dir_recursive(L4discord .)
push_sources(L4discord)

## Define fastspd executable ##

add_library(L4discord STATIC ${L4discord_SOURCES})


## Dependencies ##

# Json #
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(L4discord PUBLIC ${RAPIDJSON_INCLUDE_DIRS})


## Properties ##

# C++20 #
set_property(TARGET L4discord PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(L4discord)
