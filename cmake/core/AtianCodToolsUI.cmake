# AtianCodToolsUI
if (NOT NO_QT_BUILD)
    file(GLOB_RECURSE ACTS_COMMON_UI_SOURCES
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/resources/*"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/*.h"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/ExeDumperWidget.ui"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/ExeDllInjectorWidget.ui"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/HashTableWidget.ui"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/UI3MdiArea.cpp"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/UI3MdiArea.h"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/HashTableModel.cpp"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/widgets/HashTableModel.h"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui/MainWindow.ui"
    )

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOGEN_VERBOSE ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    qt_add_library(AtianCodToolsUI SHARED ${ACTS_COMMON_UI_SOURCES})
    source_group(
        TREE "${CMAKE_SOURCE_DIR}"
        FILES ${ACTS_COMMON_UI_SOURCES}
    )
    set_target_properties(AtianCodToolsUI PROPERTIES
        OUTPUT_NAME "acts-common-ui"
        FOLDER "Core"
    )

    target_include_directories(AtianCodToolsUI PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/core/acts-ui"
        "${CMAKE_SOURCE_DIR}/src/core/shared"
        "${CMAKE_SOURCE_DIR}/deps/mstch/include/"
        "${CMAKE_SOURCE_DIR}/deps/magic_enum/include/"
    )

    append_common_defs(AtianCodToolsUI QT)

    target_link_libraries(AtianCodToolsUI PRIVATE
        AtianCodTools
        ACTSSharedLibrary
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        mstch
    )

    add_dependencies(AtianCodToolsUI 
        AtianCodTools 
        ACTSSharedLibrary
        mstch
    )
    target_compile_definitions(AtianCodToolsUI PRIVATE 
        ACTS_COMMON_UI_DLL
    )
endif()