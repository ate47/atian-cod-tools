# AtianCodToolsCordycep
file(GLOB_RECURSE ACTS_CORDYCEP_SOURCES
    "${CMAKE_SOURCE_DIR}/resources/cordycep/*"
    "${CMAKE_SOURCE_DIR}/src/exe/cordycep/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/cordycep/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/cordycep/*.cpp"
)
add_executable(AtianCodToolsCordycep ${ACTS_CORDYCEP_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_CORDYCEP_SOURCES}
)
set_target_properties(AtianCodToolsCordycep PROPERTIES
    OUTPUT_NAME "Cordycep.CLI"
    FOLDER "Executables"
)

target_include_directories(AtianCodToolsCordycep PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/cordycep"
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(AtianCodToolsCordycep PRIVATE AtianCodTools)
add_dependencies(AtianCodToolsCordycep AtianCodTools)
append_common_defs(AtianCodToolsCordycep)
