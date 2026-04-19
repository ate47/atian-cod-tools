# Gscd
file(GLOB_RECURSE GSCD_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/gscd/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/gscd/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/gscd/*.cpp"
)
add_executable(Gscd ${GSCD_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${GSCD_SOURCES}
)
set_target_properties(Gscd PROPERTIES
    OUTPUT_NAME "gscd"
    FOLDER "Executables"
)

target_include_directories(Gscd PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/gscd"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/src/core/gsc"
)

# link to library target names assumed to exist elsewhere
target_link_libraries(Gscd PRIVATE ACTSGSC ACTSSharedLibrary)
add_dependencies(Gscd ACTSGSC ACTSSharedLibrary)
append_common_defs(Gscd)