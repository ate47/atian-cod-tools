# AtianCodToolsBOCWDLL
file(GLOB_RECURSE ACTS_BOCW_DLL_SOURCES
    "${CMAKE_SOURCE_DIR}/src/dll/bocw-dll/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/dll/bocw-dll/*.h"
    "${CMAKE_SOURCE_DIR}/src/dll/bocw-dll/*.cpp"
)
add_library(AtianCodToolsBOCWDLL SHARED)
target_sources(AtianCodToolsBOCWDLL PRIVATE ${ACTS_BOCW_DLL_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/src/dll/bocw-dll"
    FILES ${ACTS_BOCW_DLL_SOURCES}
)
set_target_properties(AtianCodToolsBOCWDLL PROPERTIES
    OUTPUT_NAME "acts-bocw"
    FOLDER "Game DLLs"
)

target_include_directories(AtianCodToolsBOCWDLL PRIVATE
    "${CMAKE_SOURCE_DIR}/src/dll/bocw-dll"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
    "${CMAKE_SOURCE_DIR}/deps/imgui"
    "${CMAKE_SOURCE_DIR}/deps/hw_break/HwBpLib/inc"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
)
append_common_defs(AtianCodToolsBOCWDLL)

target_link_libraries(AtianCodToolsBOCWDLL PRIVATE
    ACTSSharedLibrary
    detours
    asmjit
    imgui
    libcurl
)
add_dependencies(AtianCodToolsBOCWDLL
    ACTSSharedLibrary
    detours
    asmjit
    imgui
    libcurl
)
