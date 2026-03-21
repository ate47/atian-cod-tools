cmake_minimum_required(VERSION 3.20)
project(AtianCodTools LANGUAGES CXX C)

# Define build options
option(CI_BUILD "CI build define" OFF)
option(GPL_BUILD "GPL build define" OFF)
option(PRERELEASE_BUILD "Prerelease build define" OFF)

# Read version information from the version file
file(STRINGS "${CMAKE_SOURCE_DIR}/release/version" _VER_LINES)
list(GET _VER_LINES 0 VERSION)
list(GET _VER_LINES 1 VERSION_ID)

message(STATUS "Building data for acts v${VERSION} (0x${VERSION_ID})")

# Generate version information
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/actsinfo.hpp.in"
    "${CMAKE_SOURCE_DIR}/src/core/shared/core/actsinfo.hpp"
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
load_dir_recurse(dll)
load_dir_recurse(exe)

