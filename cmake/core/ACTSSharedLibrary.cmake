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

add_error_gen_target(T89Errors 
    ${CMAKE_SOURCE_DIR}/config/errors_t89.tsv 
    ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo4/t8_errors_generated.hpp
)

add_error_gen_target(T10Errors 
    ${CMAKE_SOURCE_DIR}/config/errors_t10.tsv 
    ${CMAKE_SOURCE_DIR}/src/core/shared/games/bo6/t10_errors_generated.hpp
)


if(ALL_PLATFORM_ACTS)
    target_include_directories(ACTSSharedLibrary PRIVATE
        "${CMAKE_SOURCE_DIR}/src/core/shared"
        "${CMAKE_SOURCE_DIR}/deps/curl/include"
        "${CMAKE_SOURCE_DIR}/deps/miniz-cpp"
        "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib"
    )
    append_common_defs(ACTSSharedLibrary)

    target_link_libraries(ACTSSharedLibrary PRIVATE
        libcurl
        lz4
        zstd
        zlib
        RapidJSON
        magic_enum::magic_enum
    )
    add_dependencies(ACTSSharedLibrary
        T89Errors
        T10Errors
    )
else()
    target_include_directories(ACTSSharedLibrary PRIVATE
        "${CMAKE_SOURCE_DIR}/src/core/shared"
        "${CMAKE_SOURCE_DIR}/deps/curl/include"
        "${CMAKE_SOURCE_DIR}/deps/miniz-cpp"
        "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib"
        "${CMAKE_SOURCE_DIR}/deps/ps4debug/kdebugger/include"
        "${CMAKE_SOURCE_DIR}/deps/ps4debug/ps4-ksdk/include"
        "${CMAKE_SOURCE_DIR}/deps/crc_cpp/include"
    )
    append_common_defs(ACTSSharedLibrary)

    target_link_libraries(ACTSSharedLibrary PRIVATE
        asmjit::asmjit
        detours
        libcurl
        lz4
        zstd
        zlib
        RapidJSON
        magic_enum::magic_enum
    )
    add_dependencies(ACTSSharedLibrary
        T89Errors
        T10Errors
    )
endif()

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/src/core/shared/
        ${CMAKE_SOURCE_DIR}/deps/miniz-cpp
    DESTINATION include
)
target_include_directories(ACTSSharedLibrary
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/core/shared/>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/deps/miniz-cpp>
        $<INSTALL_INTERFACE:include>
)

install_common_defs(ACTSSharedLibrary ACTS::)