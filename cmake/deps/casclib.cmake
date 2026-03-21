# casclib
file(GLOB_RECURSE CASCLIB_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/*.h"
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/*.c"
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/*.def"
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/*.rc"
)
list(REMOVE_ITEM CASCLIB_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/overwatch/cmf-key.cpp"
)
add_library(casclib STATIC)
target_sources(casclib PRIVATE ${CASCLIB_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/casclib"
    FILES ${CASCLIB_SOURCES}
)
target_compile_options(casclib PRIVATE /W0)
set_target_properties(casclib PROPERTIES
    OUTPUT_NAME "CascLib"
    FOLDER "Dependencies"
)

target_include_directories(casclib PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/casclib/src"
)
append_common_defs(casclib)
target_compile_definitions(casclib PRIVATE 
    _LIB
    _7ZIP_ST
    BZ_STRICT_ANSI
    CASCLIB_NO_AUTO_LINK_LIBRARY
)
