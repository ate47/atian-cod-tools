set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CASC_BUILD_SHARED_LIB OFF CACHE BOOL "" FORCE)
set(CASC_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
set(CASC_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(
    "${CMAKE_SOURCE_DIR}/deps/casclib"
    "${CMAKE_BINARY_DIR}/casclib"
    EXCLUDE_FROM_ALL
)

add_library(casclib ALIAS casc_static)

set_target_properties(casc_static PROPERTIES
    OUTPUT_NAME "CascLib"
    FOLDER "Dependencies"
)

target_compile_options(casc_static PRIVATE /W0)
target_compile_definitions(casc_static PRIVATE
    _LIB
    CASCLIB_NO_AUTO_LINK_LIBRARY
)
