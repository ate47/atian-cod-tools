# AtianCodToolsMW23DLL
file(GLOB_RECURSE ACTS_MW23_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/mw23-dll/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/mw23-dll/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/mw23-dll/*.cpp"
)
add_library(AtianCodToolsMW23DLL SHARED)
target_sources(AtianCodToolsMW23DLL PRIVATE ${ACTS_MW23_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/mw23-dll"
    FILES ${ACTS_MW23_DLL_SOURCES}
)
set_target_properties(AtianCodToolsMW23DLL PROPERTIES
    OUTPUT_NAME "acts-mw23"
    FOLDER "Game DLLs"
)

target_include_directories(AtianCodToolsMW23DLL PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/mw23-dll"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/"
    "${CMAKE_SOURCE_DIR}/deps/curl/include/"
)
append_common_defs(AtianCodToolsMW23DLL)

target_link_libraries(AtianCodToolsMW23DLL PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
add_dependencies(AtianCodToolsMW23DLL
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
