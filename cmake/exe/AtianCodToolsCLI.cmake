# AtianCodToolsCLI
file(GLOB_RECURSE ACTS_CLI_SOURCES
    "${CMAKE_SOURCE_DIR}/resources/cli/*"
    "${CMAKE_SOURCE_DIR}/src/exe/cli/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/cli/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/cli/*.cpp"
)
add_executable(AtianCodToolsCLI ${ACTS_CLI_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_CLI_SOURCES}
)
set_target_properties(AtianCodToolsCLI PROPERTIES
    OUTPUT_NAME "acts"
    FOLDER "Executables"
)

target_include_directories(AtianCodToolsCLI PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/cli"
    "${CMAKE_SOURCE_DIR}/include"
)

# link to library target names assumed to exist elsewhere
target_link_libraries(AtianCodToolsCLI PRIVATE AtianCodTools)
add_dependencies(AtianCodToolsCLI AtianCodTools)
append_common_defs(AtianCodToolsCLI)