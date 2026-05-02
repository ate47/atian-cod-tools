# asmjit
file(GLOB_RECURSE ASMJIT_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/*.h"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/*.cpp"
)
add_library(asmjit STATIC)
target_sources(asmjit PRIVATE ${ASMJIT_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/asmjit"
    FILES ${ASMJIT_SOURCES}
)
target_compile_options(asmjit PRIVATE /W0)
set_target_properties(asmjit PROPERTIES
    OUTPUT_NAME "asmjit"
    FOLDER "Dependencies"
)

target_include_directories(asmjit PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
)
append_common_defs(asmjit)
target_compile_definitions(asmjit PRIVATE 
    ASMJIT_STATIC
    ASMJIT_NO_AARCH64
    ASMJIT_BUILD_RELEASE
    ASMJIT_NO_FOREIGN
)

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/deps/asmjit/src
    DESTINATION include
)
install_common_defs(asmjit "")