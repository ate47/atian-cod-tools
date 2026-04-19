# ACTSGSC
file(GLOB_RECURSE ACTS_GSC_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/gsc/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/core/gsc/*.h"
    "${CMAKE_SOURCE_DIR}/src/core/gsc/*.cpp"
)
add_library(ACTSGSC STATIC)
target_sources(ACTSGSC PRIVATE ${ACTS_GSC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/core/gsc"
    FILES ${ACTS_GSC_SOURCES}
)
set_target_properties(ACTSGSC PROPERTIES
    OUTPUT_NAME "acts-gsc"
    FOLDER "Core"
)

target_include_directories(ACTSGSC PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/gsc"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
)
append_common_defs(ACTSGSC)
target_link_libraries(ACTSGSC PRIVATE
    ACTSSharedLibrary
)
add_dependencies(ACTSGSC
    ACTSSharedLibrary
)
