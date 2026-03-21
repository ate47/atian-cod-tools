# xxhash
file(GLOB_RECURSE XXHASH_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/xxHash/xxhash.c"
    "${CMAKE_SOURCE_DIR}/deps/xxHash/xxhash.h"
)
add_library(xxhash STATIC)
target_sources(xxhash PRIVATE ${XXHASH_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/xxHash"
    FILES ${XXHASH_SOURCES}
)
target_compile_options(xxhash PRIVATE /W0)
set_target_properties(xxhash PROPERTIES
    OUTPUT_NAME "xxhash"
    FOLDER "Dependencies"
)

target_include_directories(xxhash PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/xxHash/"
)
append_common_defs(xxhash C)
