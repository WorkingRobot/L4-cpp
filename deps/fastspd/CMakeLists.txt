## Source Files ##

add_dir_recursive(fastspd .)
push_sources(fastspd)

## Define fastspd executable ##

add_library(fastspd STATIC ${fastspd_SOURCES})


## Dependencies ##

target_link_libraries(fastspd PRIVATE setupapi rpcrt4)

## Properties ##

# C++20 #
set_property(TARGET fastspd PROPERTY CXX_STANDARD 23)

# Add .pdb for release builds #
force_enable_pdbs(fastspd)
