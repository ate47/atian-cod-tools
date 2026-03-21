# mstch
file(GLOB_RECURSE MSTCH_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/mstch/include/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/mstch/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/mstch/src/*.cpp"
)
add_library(mstch STATIC)
target_sources(mstch PRIVATE ${MSTCH_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/mstch"
    FILES ${MSTCH_SOURCES}
)
target_compile_options(mstch PRIVATE /W0)
set_target_properties(mstch PROPERTIES
    OUTPUT_NAME "mstch"
    FOLDER "Dependencies"
)

target_include_directories(mstch PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/mstch/include"
    "${CMAKE_SOURCE_DIR}/deps/mstch/src"
)
append_common_defs(mstch)
