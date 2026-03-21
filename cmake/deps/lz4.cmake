# lz4
file(GLOB_RECURSE LZ4_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/lz4.c"
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/lz4.h"
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/lz4hc.c"
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/lz4hc.h"
)
add_library(lz4 STATIC)
target_sources(lz4 PRIVATE ${LZ4_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/lz4/lib"
    FILES ${LZ4_SOURCES}
)
target_compile_options(lz4 PRIVATE /W0)
set_target_properties(lz4 PROPERTIES
    OUTPUT_NAME "lz4"
    FOLDER "Dependencies"
)

target_include_directories(lz4 PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/"
)
append_common_defs(lz4 C)
