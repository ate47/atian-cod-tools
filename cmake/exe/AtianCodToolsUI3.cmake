# AtianCodToolsUI3
if (NOT NO_QT_BUILD)
    file(GLOB_RECURSE ACTS_UI3_SOURCES
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/resources/*"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.h"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/widgets/ExeDumperWidget.ui"
        "${CMAKE_SOURCE_DIR}/resources/ui/*"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3/MainWindow.ui"
    )

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOGEN_VERBOSE ON)
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
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/exe/ui3"
        "${CMAKE_SOURCE_DIR}/src/core/shared"
        "${CMAKE_SOURCE_DIR}/deps/mstch/include/"
    )

    append_common_defs(AtianCodToolsUI3 QT)

    target_link_libraries(AtianCodToolsUI3 PRIVATE
        AtianCodTools
        ACTSSharedLibrary
        Qt6::Core
        Qt6::Widgets
        mstch
    )

    add_dependencies(AtianCodToolsUI3 
        AtianCodTools 
        ACTSSharedLibrary
        mstch
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

            COMMAND "${QT_BIN_DIR}/windeployqt.exe"
                    --no-compiler-runtime
                    --no-translations
                    --release
                    "$<TARGET_FILE:AtianCodToolsUI3>"
                            
            COMMAND ${CMAKE_COMMAND} -E echo "Remove data in $<TARGET_FILE_DIR:AtianCodToolsUI3>"
            COMMAND ${CMAKE_COMMAND} -E rm -rf 
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/imageformats/qjpeg.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/imageformats/qgif.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/Qt6Network.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/Qt6Svg.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/D3Dcompiler_47.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/dxcompiler.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/opengl32sw.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/icuuc.dll"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/networkinformation"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/tls"
                "$<TARGET_FILE_DIR:AtianCodToolsUI3>/generic"
        )
    endif()
endif()