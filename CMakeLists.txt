cmake_minimum_required(VERSION 3.20)
project(AtianCodTools LANGUAGES CXX C)

# Define build options
option(CI_BUILD "CI build define" OFF)
option(GPL_BUILD "GPL build define" OFF)
option(PRERELEASE_BUILD "Prerelease build define" OFF)
option(NO_QT_BUILD "Disable QT projects" OFF)
option(STATIC_ACTS "acts-common is compiled as a static library" OFF)

if (NOT NO_QT_BUILD)
    find_package(Qt6 REQUIRED COMPONENTS Core)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

# Read version information from the version file
file(STRINGS "${CMAKE_SOURCE_DIR}/release/version" _VER_LINES)
list(GET _VER_LINES 0 VERSION)
list(GET _VER_LINES 1 VERSION_ID)

file(STRINGS "${CMAKE_SOURCE_DIR}/release/updater" _UPDATER_LINES)
list(GET _UPDATER_LINES 0 RELEASE_VERSION_FILE)
list(GET _UPDATER_LINES 1 RELEASE_ZIP_FILE)
list(GET _UPDATER_LINES 2 RELEASE_PROJECT)
list(GET _UPDATER_LINES 3 RELEASE_PROJECT_URL)

if(CI_BUILD)
    set(ACTSAVI_DEV "")
    set(ACTSAVI_LDEV "")
else()
    set(ACTSAVI_DEV "dev-")
    set(ACTSAVI_LDEV "L\"dev-\"")
endif()

if(GPL_BUILD)
    set(ACTSAVI_GPL "-gpl")
    set(ACTSAVI_LGPL "L\"-gpl\"")
else()
    set(ACTSAVI_GPL "")
    set(ACTSAVI_LGPL "")
endif()

if(PRERELEASE_BUILD)
    set(ACTSAVI_PRER "-pre")
    set(ACTSAVI_LPRER "L\"-pre\"")
else()
    set(ACTSAVI_PRER "")
    set(ACTSAVI_LPRER "")
endif()

set(ACTS_VERSION_STRING
    "${ACTSAVI_DEV}${VERSION}${ACTSAVI_GPL}${ACTSAVI_PRER}"
)

message(STATUS "Building data for acts ${ACTS_VERSION_STRING} (0x${VERSION_ID})")

# Generate version information
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/updater_endpoint.hpp.in"
    "${CMAKE_SOURCE_DIR}/src/core/shared/core/updater_endpoint.hpp"
)
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/actsinfo.hpp.in"
    "${CMAKE_SOURCE_DIR}/src/core/shared/core/actsinfo.hpp"
)
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/version_info.h.in"
    "${CMAKE_SOURCE_DIR}/include/acts_api/version_info.h"
)
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/xsk_version.hpp.in"
    "${CMAKE_SOURCE_DIR}/deps/gsc-tool/include/xsk/version.hpp"
)

# Load solution projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

macro(append_common_defs target)
    set(target_type "${ARGV1}")

    if ("${target_type}" STREQUAL "")
        set(target_type "CXX")
    endif()

    target_compile_definitions(${target} PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<BOOL:${CI_BUILD}>:CI_BUILD>
        $<$<BOOL:${GPL_BUILD}>:GPL_BUILD>
        $<$<BOOL:${PRERELEASE_BUILD}>:PRERELEASE_BUILD>
    )
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin"
        OBJECT_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/build/obj"
    )
    if ("${target_type}" STREQUAL "CXX")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
        )
        target_compile_options(${target} PRIVATE "/source-charset:windows-1252")
    elseif ("${target_type}" STREQUAL "QT")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
        )
        qt_import_plugins("${target_type}")
    elseif("${target_type}" STREQUAL "C")
        set_target_properties(${target} PROPERTIES 
            LINKER_LANGUAGE C
        )
    endif()

endmacro()

macro(load_dir_recurse id)
    file(GLOB DEP_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/${id}/*.cmake")

    foreach(script ${DEP_SCRIPTS})
        message(STATUS "Loading ${script}")
        include(${script})
    endforeach()
endmacro()

load_dir_recurse(deps)
if (GPL_BUILD)
    load_dir_recurse(deps-gpl)
endif()
load_dir_recurse(core)
if (!STATIC_ACTS) {
    # we do link the dll/exe only if the lib is not included
    load_dir_recurse(dll)
    load_dir_recurse(exe)
}

