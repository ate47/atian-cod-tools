# detours
file(GLOB_RECURSE DETOURS_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/detours.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/modules.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disasm.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/image.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/creatwth.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disolx86.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disolx64.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disolia64.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disolarm.*"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/disolarm64.*"
)
add_library(detours STATIC)
target_sources(detours PRIVATE ${DETOURS_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/Detours"
    FILES ${DETOURS_SOURCES}
)
target_compile_options(detours PRIVATE /W0)
set_target_properties(detours PROPERTIES
    OUTPUT_NAME "detours"
    FOLDER "Dependencies"
)

target_include_directories(detours PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
)
append_common_defs(detours)
