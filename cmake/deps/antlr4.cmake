# antlr4-runtime
file(GLOB_RECURSE ANTLR4_RUNTIME_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src/*.h"
    "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src/*.cpp"
)
add_library(antlr4-runtime STATIC)
target_sources(antlr4-runtime PRIVATE ${ANTLR4_RUNTIME_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src"
    FILES ${ANTLR4_RUNTIME_SOURCES}
)
target_compile_options(antlr4-runtime PRIVATE /W0)
set_target_properties(antlr4-runtime PROPERTIES
    OUTPUT_NAME "antlr4-runtime"
    FOLDER "Dependencies"
)

target_include_directories(antlr4-runtime PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src"
)
append_common_defs(antlr4-runtime)
target_compile_definitions(antlr4-runtime PRIVATE 
    ANTLR4CPP_STATIC
)
