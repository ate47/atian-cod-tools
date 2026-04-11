# AtianCodTools
file(GLOB_RECURSE ACTS_SOURCES
    "${CMAKE_SOURCE_DIR}/src/core/acts/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/acts/*.h"
    "${CMAKE_SOURCE_DIR}/src/core/acts/*.hpp"
)
file(GLOB_RECURSE ACTS_INCLUDE_SOURCES
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)
if(STATIC_ACTS)
    add_library(AtianCodTools SHARED)
    set_target_properties(AtianCodTools PROPERTIES
        OUTPUT_NAME "acts-common"
        FOLDER "Core"
    )
else()
    add_library(AtianCodTools STATIC)
    set_target_properties(AtianCodTools PROPERTIES
        OUTPUT_NAME "acts-common-static"
        FOLDER "Core"
    )
endif()

target_sources(AtianCodTools PRIVATE ${ACTS_SOURCES} ${ACTS_INCLUDE_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/core/acts"
    PREFIX acts
    FILES ${ACTS_SOURCES}
)
source_group(
    TREE "${CMAKE_SOURCE_DIR}/include"
    PREFIX include
    FILES ${ACTS_INCLUDE_SOURCES}
)
target_precompile_headers(AtianCodTools PRIVATE "${CMAKE_SOURCE_DIR}/src/core/acts/includes.hpp")
append_common_defs(AtianCodTools)
target_include_directories(AtianCodTools PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/acts"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/src/core/lib"
    "${CMAKE_SOURCE_DIR}/src/core/vm"
    "${CMAKE_SOURCE_DIR}/deps/antlr4/runtime/Cpp/runtime/src/"
    "${CMAKE_SOURCE_DIR}/deps/zlib/"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp/include/"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/kdebugger/include/"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/ps4-ksdk/include/"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/"
    "${CMAKE_SOURCE_DIR}/deps/curl/include/"
    "${CMAKE_SOURCE_DIR}/deps/casclib/src/"
    "${CMAKE_SOURCE_DIR}/deps/lz4/lib/"
    "${CMAKE_SOURCE_DIR}/deps/zstd/lib/"
    "${CMAKE_SOURCE_DIR}/deps/salsa20/Source/"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include/"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib/"
    "${CMAKE_SOURCE_DIR}/deps/json-rpc-cxx/include/"
    "${CMAKE_SOURCE_DIR}/deps/json/include/"
    "${CMAKE_SOURCE_DIR}/deps/rapidcsv/src/"
    "${CMAKE_SOURCE_DIR}/deps/hw_break/HwBpLib/inc/"
    "${CMAKE_SOURCE_DIR}/deps/glfw/include/"
    "${CMAKE_SOURCE_DIR}/deps/imgui/"
    "${CMAKE_SOURCE_DIR}/deps/thread-pool/include/"
    "${CMAKE_SOURCE_DIR}/deps/stb/"
    "${CMAKE_SOURCE_DIR}/deps/tomlplusplus/include/toml++/"
    "${CMAKE_SOURCE_DIR}/deps/miniz-cpp/"
    "${CMAKE_SOURCE_DIR}/deps/hash-library/"
    "${CMAKE_SOURCE_DIR}/deps/hksc/etc/"
    "${CMAKE_SOURCE_DIR}/deps/hksc/src/"
    "${CMAKE_SOURCE_DIR}/deps/crc_cpp/include/"
    "${CMAKE_SOURCE_DIR}/deps/libtomcrypt/src/headers/"
    "${CMAKE_SOURCE_DIR}/deps/mstch/include/"
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(AtianCodTools PRIVATE
    antlr4-runtime
    ACTSSharedLibrary
    ACTSLibrary
    ACTSVM
    zlib
    libps4debug
    asmjit
    casclib
    lz4
    zstd
    detours
    libcurl
    glfw
    OpenGL32
    GLU32
    imgui
    Crypt32.lib
    hashlibrary
    xxhash
    hksc
    libtomcrypt
    mstch
)

add_dependencies(AtianCodTools
    detours
    antlr4-runtime
    ACTSSharedLibrary
    ACTSLibrary
    ACTSVM
    zlib
    libps4debug
    asmjit
    casclib
    lz4
    zstd
    glfw
    imgui
    hashlibrary
    xxhash
    hksc
    libtomcrypt
    mstch
    libcurl
)
target_compile_definitions(AtianCodTools PRIVATE 
    ANTLR4CPP_STATIC
    CASCLIB_NO_AUTO_LINK_LIBRARY
    ACTS_COMMON_DLL
    LTM_DESC
)

if (GPL_BUILD)
    target_link_libraries(AtianCodTools PRIVATE
        xsk-arc
        xsk-gsc
    )
    add_dependencies(AtianCodTools
        xsk-arc
        xsk-gsc
    )
    target_include_directories(AtianCodTools PRIVATE
        "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include"
    )
endif()