# libtomcrypt
file(GLOB_RECURSE LIBTOMCRYPT_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/libtomcrypt/src/*.c"
    "${CMAKE_SOURCE_DIR}/deps/libtomcrypt/src/*.h"
)
add_library(libtomcrypt STATIC)
target_sources(libtomcrypt PRIVATE ${LIBTOMCRYPT_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/libtomcrypt"
    FILES ${LIBTOMCRYPT_SOURCES}
)
target_compile_options(libtomcrypt PRIVATE /W0)
set_target_properties(libtomcrypt PROPERTIES
    OUTPUT_NAME "libtomcrypt"
    FOLDER "Dependencies"
)

target_include_directories(libtomcrypt PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/libtomcrypt/src/headers/"
    "${CMAKE_SOURCE_DIR}/deps/libtommath/"
)
append_common_defs(libtomcrypt C)
target_link_libraries(libtomcrypt PRIVATE
    libtommath
)
add_dependencies(libtomcrypt
    libtommath
)
target_compile_definitions(libtomcrypt PRIVATE 
    LTM_DESC
)
