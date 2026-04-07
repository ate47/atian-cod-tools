# AtianCodToolsUI (WindowedApp equivalent -> WIN32)
file(GLOB_RECURSE ACTS_UI_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/ui/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/ui/*.cpp"
    "${CMAKE_SOURCE_DIR}/resources/ui/*"
)
add_executable(AtianCodToolsUI WIN32 ${ACTS_UI_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_UI_SOURCES}
)
set_target_properties(AtianCodToolsUI PROPERTIES
    OUTPUT_NAME "acts-ui"
    FOLDER "Executables"
)

target_include_directories(AtianCodToolsUI PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/exe/ui"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
    "${CMAKE_SOURCE_DIR}/deps/ps4debug/libdebug/cpp/include"
    "${CMAKE_SOURCE_DIR}/deps/asmjit/src"
    "${CMAKE_SOURCE_DIR}/deps/curl/include"
    "${CMAKE_SOURCE_DIR}/deps/Detours/src"
)

# Link presumed targets (create IMPORTED or add_subdirectory() for these deps elsewhere)
target_link_libraries(AtianCodToolsUI PRIVATE
    AtianCodTools
    ACTSSharedLibrary
    libps4debug
    asmjit
    detours
    libcurl
)

add_dependencies(AtianCodToolsUI 
    AtianCodTools 
    ACTSSharedLibrary
    libps4debug
    asmjit
    detours
    libcurl
)
append_common_defs(AtianCodToolsUI)
