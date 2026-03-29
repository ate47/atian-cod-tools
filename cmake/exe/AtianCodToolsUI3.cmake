# AtianCodToolsUI3
file(GLOB_RECURSE ACTS_UI3_SOURCES
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/icons/*"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.h"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.ui"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.qrc"
    "${CMAKE_SOURCE_DIR}/resources/ui/*"
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable(AtianCodToolsUI3 ${ACTS_UI3_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}"
    FILES ${ACTS_UI3_SOURCES}
)
set_target_properties(AtianCodToolsUI3 PROPERTIES
    OUTPUT_NAME "acts-ui3"
    FOLDER "Executables"
    WIN32_EXECUTABLE ON
)

target_include_directories(AtianCodToolsUI3 PRIVATE
    "${CMAKE_SOURCE_DIR}/src/core/acts"
    "${CMAKE_SOURCE_DIR}/src/exe/ui3"
    "${CMAKE_SOURCE_DIR}/src/core/shared"
)

append_common_defs(AtianCodToolsUI3 QT)

target_link_libraries(AtianCodToolsUI3 PRIVATE
    AtianCodTools
    ACTSSharedLibrary
    Qt6::Core
    Qt6::Widgets
)

add_dependencies(AtianCodToolsUI3 
    AtianCodTools 
    ACTSSharedLibrary
)

get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

# This should be done in the packaging, not in the release dir for deployment
if (CI_BUILD)
    # todo: add deploy for ci?
else()
    add_custom_command(TARGET AtianCodToolsUI3 POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                --version
    )
    add_custom_command(TARGET AtianCodToolsUI3 POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                --no-compiler-runtime
                --no-translations
                --release
                "$<TARGET_FILE:AtianCodToolsUI3>"
        COMMENT "Running windeployqt on AtianCodToolsUI3"
    )
endif()