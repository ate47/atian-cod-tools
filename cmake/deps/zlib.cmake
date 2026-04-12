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

# zlib-minizip
file(GLOB ZLIB_MZ_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/zip.c"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/zip.h"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/unzip.c"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/unzip.h"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/ioapi.c"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/ioapi.h"
)
add_library(minizip STATIC)
target_sources(minizip PRIVATE ${ZLIB_MZ_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip"
    FILES ${ZLIB_MZ_SOURCES}
)
target_compile_options(minizip PRIVATE /W0)
set_target_properties(minizip PROPERTIES
    OUTPUT_NAME "minizip"
    FOLDER "Dependencies"
)
target_include_directories(minizip PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/zlib/"
    "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip"
)
append_common_defs(minizip C)
