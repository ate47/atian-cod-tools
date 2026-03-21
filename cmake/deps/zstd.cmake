# zstd
file(GLOB_RECURSE ZSTD_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/zstd.h"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/common/*.c"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/common/*.h"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/decompress/*.c"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/decompress/*.h"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/compress/*.c"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/compress/*.h"
)
add_library(zstd STATIC)
target_sources(zstd PRIVATE ${ZSTD_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/zstd/lib"
    FILES ${ZSTD_SOURCES}
)
target_compile_options(zstd PRIVATE /W0)
set_target_properties(zstd PROPERTIES
    OUTPUT_NAME "zstd"
    FOLDER "Dependencies"
)

target_include_directories(zstd PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/common"
)
append_common_defs(zstd C)
target_compile_definitions(zstd PRIVATE 
    XXH_NAMESPACE=ZSTD_
)
target_link_libraries(zstd PRIVATE
    lz4
    zlib
)
add_dependencies(zstd
    lz4
    zlib
)
