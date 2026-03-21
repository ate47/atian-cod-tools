# AtianCodToolsBO4ShieldPlugin
file(GLOB_RECURSE ACTS_BO4_SHIELD_PLUGIN_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin/*.cpp"
)
add_library(AtianCodToolsBO4ShieldPlugin SHARED)
target_sources(AtianCodToolsBO4ShieldPlugin PRIVATE ${ACTS_BO4_SHIELD_PLUGIN_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin"
    FILES ${ACTS_BO4_SHIELD_PLUGIN_DLL_SOURCES}
)
set_target_properties(AtianCodToolsBO4ShieldPlugin PROPERTIES
    OUTPUT_NAME "acts-shield-plugin"
    FOLDER "Game DLLs"
)

target_include_directories(AtianCodToolsBO4ShieldPlugin PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/shield-plugin"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib"
    "${CMAKE_SOURCE_DIR}/deps/rapidcsv/src"
)
append_common_defs(AtianCodToolsBO4ShieldPlugin)

target_link_libraries(AtianCodToolsBO4ShieldPlugin PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
add_dependencies(AtianCodToolsBO4ShieldPlugin
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
