## Source Files ##

add_dir(SourceRiot .)
push_sources(SourceRiot)


## Define SourceRiot library ##

add_library(SourceRiot SHARED ${SourceRiot_SOURCES})


## Dependencies ##

# Source Interface #
target_include_directories(SourceRiot PRIVATE ${CMAKE_SOURCE_DIR}/src/source)

# Json #
find_package(RapidJSON CONFIG REQUIRED)
target_include_directories(SourceRiot PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

# Http #
find_package(cpr CONFIG REQUIRED)
target_link_libraries(SourceRiot PRIVATE cpr::cpr)

# Zstd #
find_package(zstd CONFIG REQUIRED)
target_link_libraries(SourceRiot PRIVATE zstd::libzstd_shared)

## Properties ##

# C++20 #
set_property(TARGET SourceRiot PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(SourceRiot)
