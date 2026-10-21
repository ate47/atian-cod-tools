# zlib-minizip
file(GLOB MINIZIP_SOURCES
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/zip.c"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/zip.h"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/unzip.c"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/unzip.h"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/ioapi.c"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip/ioapi.h"
)
add_library(minizip STATIC)
target_sources(minizip PRIVATE ${MINIZIP_SOURCES})
source_group(
	TREE "${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip"
	FILES ${MINIZIP_SOURCES}
)
target_compile_options(minizip PRIVATE /W0)
set_target_properties(minizip PROPERTIES
	OUTPUT_NAME "minizip"
	FOLDER "Dependencies"
)
target_include_directories(minizip PRIVATE
	"${CMAKE_SOURCE_DIR}/deps/zlib/"
	"${CMAKE_SOURCE_DIR}/deps/zlib/contrib/minizip"
)
append_common_defs(minizip C)
target_link_libraries(minizip PRIVATE zlib)
