## Source Files ##

add_dir_recursive(SourceRiot .)
push_sources(SourceRiot)

## Define fastspd executable ##

add_library(SourceRiot SHARED ${SourceRiot_SOURCES})


## Dependencies ##

# Source Interface #
target_compile_definitions(SourceRiot PRIVATE L4_SOURCE_BUILDING_SOURCE)
target_link_libraries(SourceRiot PRIVATE L4source)

# Json #
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(SourceRiot PRIVATE ${RAPIDJSON_INCLUDE_DIRS})


## Properties ##

# C++20 #
set_property(TARGET SourceRiot PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(SourceRiot)
