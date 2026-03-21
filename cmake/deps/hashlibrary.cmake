# hashlibrary
file(GLOB HASHLIBRARY_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/hash-library/*.h"
    "${CMAKE_SOURCE_DIR}/deps/hash-library/*.cpp"
)
add_library(hashlibrary STATIC)
target_sources(hashlibrary PRIVATE ${HASHLIBRARY_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/hash-library"
    FILES ${HASHLIBRARY_SOURCES}
)
target_compile_options(hashlibrary PRIVATE /W0)
set_target_properties(hashlibrary PROPERTIES
    OUTPUT_NAME "hashlibrary"
    FOLDER "Dependencies"
)

target_include_directories(hashlibrary PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/hash-library"
)
append_common_defs(hashlibrary)
