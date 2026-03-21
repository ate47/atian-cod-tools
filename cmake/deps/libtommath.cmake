# libtommath
file(GLOB LIBTOMMATH_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/libtommath/*.c"
    "${CMAKE_SOURCE_DIR}/deps/libtommath/*.h"
    "${CMAKE_SOURCE_DIR}/deps/libtommath/*.def"
)
add_library(libtommath STATIC)
target_sources(libtommath PRIVATE ${LIBTOMMATH_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/libtommath"
    FILES ${LIBTOMMATH_SOURCES}
)
target_compile_options(libtommath PRIVATE /W0)
set_target_properties(libtommath PROPERTIES
    OUTPUT_NAME "libtommath"
    FOLDER "Dependencies"
)

target_include_directories(libtommath PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/libtommath/"
)
append_common_defs(libtommath C)
