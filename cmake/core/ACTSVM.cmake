# ACTSVM
file(GLOB_RECURSE ACTS_VM_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/vm/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/core/vm/*.h"
    "${CMAKE_SOURCE_DIR}/src/core/vm/*.cpp"
)
add_library(ACTSVM STATIC)
target_sources(ACTSVM PRIVATE ${ACTS_VM_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/core/vm"
    FILES ${ACTS_VM_SOURCES}
)
set_target_properties(ACTSVM PROPERTIES
    OUTPUT_NAME "acts-vm"
    FOLDER "Core"
)

target_include_directories(ACTSVM PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/vm"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
)
append_common_defs(ACTSVM)
target_link_libraries(ACTSVM PRIVATE
    ACTSSharedLibrary
)
add_dependencies(ACTSVM
    ACTSSharedLibrary
)
