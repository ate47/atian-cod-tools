# libps4debug
file(GLOB_RECURSE LIBPS4DEBUG_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp/source/*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp/include/*.hpp"
)
add_library(libps4debug STATIC)
target_sources(libps4debug PRIVATE ${LIBPS4DEBUG_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp"
    FILES ${LIBPS4DEBUG_SOURCES}
)
target_compile_options(libps4debug PRIVATE /W0)
set_target_properties(libps4debug PROPERTIES
    OUTPUT_NAME "libps4debug"
    FOLDER "Dependencies"
)

target_include_directories(libps4debug PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp/include"
)
# cxx17
append_common_defs(libps4debug)