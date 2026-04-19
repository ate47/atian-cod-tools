# Gsc
file(GLOB_RECURSE GSC_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/gsc/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/gsc/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/gsc/*.cpp"
)
add_executable(Gsc ${GSC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${GSC_SOURCES}
)
set_target_properties(Gsc PROPERTIES
    OUTPUT_NAME "gsc"
    FOLDER "Executables"
)

target_include_directories(Gsc PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/gsc"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/src/core/gsc"
)

# link to library target names assumed to exist elsewhere
target_link_libraries(Gsc PRIVATE ACTSGSC ACTSSharedLibrary)
add_dependencies(Gsc ACTSGSC ACTSSharedLibrary)
append_common_defs(Gsc)