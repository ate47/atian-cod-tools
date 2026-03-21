# xsk-gsc
file(GLOB_RECURSE XEN_GSC_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/gsc/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/gsc/*.h"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/src/gsc/*.cpp"
)
add_library(xsk-gsc STATIC)
target_sources(xsk-gsc PRIVATE ${XEN_GSC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/gsc-tool"
    FILES ${XEN_GSC_SOURCES}
)
set_target_properties(xsk-gsc PROPERTIES
    OUTPUT_NAME "gsc"
    FOLDER "Dependencies"
)

target_include_directories(xsk-gsc PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include"
)
append_common_defs(xsk-gsc)
target_link_libraries(xsk-gsc PRIVATE
    xsk-utils
)
add_dependencies(xsk-gsc
    xsk-utils
)
target_compile_definitions(xsk-gsc PRIVATE 
    XSK_NO_COMPILED_HASH
)
