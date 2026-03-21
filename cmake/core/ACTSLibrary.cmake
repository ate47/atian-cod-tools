# ACTSLibrary
file(GLOB_RECURSE ACTS_LIB_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/lib/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/core/lib/*.h"
    "${CMAKE_SOURCE_DIR}/src/core/lib/*.cpp"
)
add_library(ACTSLibrary STATIC)
target_sources(ACTSLibrary PRIVATE ${ACTS_LIB_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/core/lib"
    FILES ${ACTS_LIB_SOURCES}
)
set_target_properties(ACTSLibrary PROPERTIES
    OUTPUT_NAME "actslib"
    FOLDER "Core"
)

target_include_directories(ACTSLibrary PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/lib"
)
append_common_defs(ACTSLibrary)
