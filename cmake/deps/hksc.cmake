# hksc
file(GLOB_RECURSE HKSC_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/hksc/src/*.c"
    "${CMAKE_SOURCE_DIR}/deps/hksc/src/*.def"
    "${CMAKE_SOURCE_DIR}/deps/hksc/src/*.h"
    "${CMAKE_SOURCE_DIR}/deps/hksc/src/*.hpp"
)
add_library(hksc STATIC)
target_sources(hksc PRIVATE ${HKSC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/hksc"
    FILES ${HKSC_SOURCES}
)
target_compile_options(hksc PRIVATE /W0)
set_target_properties(hksc PROPERTIES
    OUTPUT_NAME "hksc"
    FOLDER "Dependencies"
)

target_include_directories(hksc PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/hksc/src"
)
append_common_defs(hksc C)
