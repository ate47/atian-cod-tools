# AtianCodToolsCOD2020DLL
file(GLOB_RECURSE ACTS_BO2020_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/bo2020-dll/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/bo2020-dll/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/bo2020-dll/*.cpp"
)
add_library(AtianCodToolsCOD2020DLL SHARED)
target_sources(AtianCodToolsCOD2020DLL PRIVATE ${ACTS_BO2020_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/bo2020-dll"
    FILES ${ACTS_BO2020_DLL_SOURCES}
)
set_target_properties(AtianCodToolsCOD2020DLL PROPERTIES
    OUTPUT_NAME "acts-cod2020"
    FOLDER "Game DLLs"
)

target_include_directories(AtianCodToolsCOD2020DLL PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/bo2020-dll"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src/"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src/"
    "${CMAKE_SOURCE_DIR}/deps/curl/include/"
    "${CMAKE_SOURCE_DIR}/deps/rapidjson/include/"
    "${CMAKE_SOURCE_DIR}/deps/dbflib/src/lib/"
)
append_common_defs(AtianCodToolsCOD2020DLL)

target_link_libraries(AtianCodToolsCOD2020DLL PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
add_dependencies(AtianCodToolsCOD2020DLL
    ACTSSharedLibrary
    detours
    asmjit
    libcurl
)
