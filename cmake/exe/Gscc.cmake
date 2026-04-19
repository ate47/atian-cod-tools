# Gscc
file(GLOB_RECURSE GSCC_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/gscc/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/gscc/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/gscc/*.cpp"
)
add_executable(Gscc ${GSCC_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${GSCC_SOURCES}
)
set_target_properties(Gscc PROPERTIES
    OUTPUT_NAME "gscc"
    FOLDER "Executables"
)

target_include_directories(Gscc PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/gscc"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/src/core/gsc"
)

# link to library target names assumed to exist elsewhere
target_link_libraries(Gscc PRIVATE ACTSGSC ACTSSharedLibrary)
add_dependencies(Gscc ACTSGSC ACTSSharedLibrary)
append_common_defs(Gscc)