# zlib
file(GLOB ZLIB_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/zlib/*.c"
    "${CMAKE_SOURCE_DIR}/deps/zlib/*.h"
)
add_library(zlib STATIC)
target_sources(zlib PRIVATE ${ZLIB_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/zlib"
    FILES ${ZLIB_SOURCES}
)
target_compile_options(zlib PRIVATE /W0)
set_target_properties(zlib PROPERTIES
    OUTPUT_NAME "zlib"
    FOLDER "Dependencies"
)
append_common_defs(zlib C)
