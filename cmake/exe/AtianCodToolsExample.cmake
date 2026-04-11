# AtianCodToolsExample
file(GLOB_RECURSE ACTS_EXAMPLE_SOURCES
    "${CMAKE_SOURCE_DIR}/examples/*.h"
    "${CMAKE_SOURCE_DIR}/examples/*.c"
)
add_executable(AtianCodToolsExample ${ACTS_EXAMPLE_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_EXAMPLE_SOURCES}
)
set_target_properties(AtianCodToolsExample PROPERTIES
    OUTPUT_NAME "test-acts-example"
    FOLDER "Executables"
)

target_include_directories(AtianCodToolsExample PRIVATE
    "${CMAKE_SOURCE_DIR}/example"
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(AtianCodToolsExample PRIVATE AtianCodTools)
add_dependencies(AtianCodToolsExample AtianCodTools)
append_common_defs(AtianCodToolsExample C)
