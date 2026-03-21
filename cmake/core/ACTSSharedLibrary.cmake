# ACTSSharedLibrary
file(GLOB_RECURSE ACTS_SHARED_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/shared/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/core/shared/*.h"
    "${CMAKE_SOURCE_DIR}/src/core/shared/*.cpp"
)
add_library(ACTSSharedLibrary STATIC)
target_sources(ACTSSharedLibrary PRIVATE ${ACTS_SHARED_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/core/shared"
    FILES ${ACTS_SHARED_SOURCES}
)
set_target_properties(ACTSSharedLibrary PROPERTIES
    OUTPUT_NAME "actsshared"
    FOLDER "Core"
)
target_precompile_headers(ACTSSharedLibrary PRIVATE "${CMAKE_SOURCE_DIR}/src/core/shared/includes_shared.hpp")

target_include_directories(ACTSSharedLibrary PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include"
    "${CMAKE_SOURCE_DIR}/deps/miniz-cpp"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib"
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib"
    "${CMAKE_SOURCE_DIR}/deps/zlib"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/kdebugger/include"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/ps4-ksdk/include"
    "${CMAKE_SOURCE_DIR}/deps/crc_cpp/include"
)
append_common_defs(ACTSSharedLibrary)

target_link_libraries(ACTSSharedLibrary PRIVATE
    asmjit
    detours
    libcurl
    lz4
    zstd
    zlib
)
add_dependencies(ACTSSharedLibrary
    asmjit
    detours
    libcurl
    lz4
    zstd
    zlib
)
