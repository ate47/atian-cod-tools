# AtianCodToolsUpdater
file(GLOB_RECURSE ACTS_UPDATER_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/updater/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/updater/*.cpp"
    "${CMAKE_SOURCE_DIR}/resources/updater/*"
)
add_executable(AtianCodToolsUpdater ${ACTS_UPDATER_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_UPDATER_SOURCES}
)
set_target_properties(AtianCodToolsUpdater PROPERTIES
    OUTPUT_NAME "acts-updater"
    FOLDER "Executables"
)

target_include_directories(AtianCodToolsUpdater PRIVATE
    "${CMAKE_SOURCE_DIR}/src/exe/updater"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
)

target_link_libraries(AtianCodToolsUpdater PRIVATE ACTSSharedLibrary)
add_dependencies(AtianCodToolsUpdater ACTSSharedLibrary)
append_common_defs(AtianCodToolsUpdater)
