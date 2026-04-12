# AtianCodToolsBO3DLL
file(GLOB_RECURSE ACTS_BO3_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/bo3-dll/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/bo3-dll/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/bo3-dll/*.cpp"
)
add_library(AtianCodToolsBO3DLL SHARED)
target_sources(AtianCodToolsBO3DLL PRIVATE ${ACTS_BO3_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/bo3-dll"
    FILES ${ACTS_BO3_DLL_SOURCES}
)
set_target_properties(AtianCodToolsBO3DLL PROPERTIES
    OUTPUT_NAME "acts-bo3"
    FOLDER "Game DLLs"
)

target_include_directories(AtianCodToolsBO3DLL PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/bo3-dll"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib"
    "${CMAKE_SOURCE_DIR}/deps/rapidcsv/src"
)
append_common_defs(AtianCodToolsBO3DLL)

target_link_libraries(AtianCodToolsBO3DLL PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
add_dependencies(AtianCodToolsBO3DLL
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
