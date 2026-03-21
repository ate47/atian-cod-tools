# xsk-utils
file(GLOB_RECURSE XEN_UTILS_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/utils/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/utils/*.h"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/src/utils/*.cpp"
)
add_library(xsk-utils STATIC)
target_sources(xsk-utils PRIVATE ${XEN_UTILS_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/gsc-tool"
    FILES ${XEN_UTILS_SOURCES}
)
set_target_properties(xsk-utils PROPERTIES
    OUTPUT_NAME "xsk-utils"
    FOLDER "Dependencies"
)

target_include_directories(xsk-utils PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include"
    "${CMAKE_SOURCE_DIR}/deps/zlib"
)
append_common_defs(xsk-utils)
target_link_libraries(xsk-utils PRIVATE
    zlib
)
add_dependencies(xsk-utils
    zlib
)