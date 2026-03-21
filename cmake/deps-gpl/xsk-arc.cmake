# xsk-arc
file(GLOB_RECURSE XEN_ARC_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/arc/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/arc/*.h"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/src/arc/*.cpp"
)
add_library(xsk-arc STATIC)
target_sources(xsk-arc PRIVATE ${XEN_ARC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/gsc-tool"
    FILES ${XEN_ARC_SOURCES}
)
set_target_properties(xsk-arc PROPERTIES
    OUTPUT_NAME "xsk-arc"
    FOLDER "Dependencies"
)

target_include_directories(xsk-arc PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include"
)
append_common_defs(xsk-arc)
target_link_libraries(xsk-arc PRIVATE
    xsk-utils
)
add_dependencies(xsk-arc
    xsk-utils
)
target_compile_definitions(xsk-arc PRIVATE 
    XSK_NO_COMPILED_HASH
)
