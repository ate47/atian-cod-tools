# TestDll
file(GLOB_RECURSE TEST_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/test-dll/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/test-dll/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/test-dll/*.cpp"
)
add_library(TestDll SHARED)
target_sources(TestDll PRIVATE ${TEST_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/test-dll"
    FILES ${TEST_DLL_SOURCES}
)
set_target_properties(TestDll PROPERTIES
    OUTPUT_NAME "test-dll"
    FOLDER "Game DLLs"
)

target_include_directories(TestDll PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/test-dll"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/"
    "${CMAKE_SOURCE_DIR}/deps/curl/include/"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include/"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib/"
)
append_common_defs(TestDll)

target_link_libraries(TestDll PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
add_dependencies(TestDll
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)