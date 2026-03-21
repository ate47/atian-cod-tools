# glfw
file(GLOB_RECURSE GLFW_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/glfw/include/GLFW/glfw3.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/include/GLFW/glfw3native.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/internal.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/platform.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/mappings.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/context.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/init.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/input.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/monitor.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/platform.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/vulkan.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/window.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/egl_context.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/osmesa_context.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_platform.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_joystick.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_init.c"

    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_monitor.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_window.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/null_joystick.c"

    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_init.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_module.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_joystick.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_monitor.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_time.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_time.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_thread.h"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_thread.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/win32_window.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/wgl_context.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/egl_context.c"
    "${CMAKE_SOURCE_DIR}/deps/glfw/src/osmesa_context.c"
)
add_library(glfw STATIC)
target_sources(glfw PRIVATE ${GLFW_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/glfw"
    FILES ${GLFW_SOURCES}
)
target_compile_options(glfw PRIVATE /W0)
set_target_properties(glfw PROPERTIES
    OUTPUT_NAME "glfw"
    FOLDER "Dependencies"
)

target_include_directories(glfw PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/glfw/src"
    "${CMAKE_SOURCE_DIR}/deps/glfw/include"
)
append_common_defs(glfw C)
target_compile_definitions(glfw PRIVATE 
    _GLFW_WIN32
    _CRT_SECURE_NO_WARNINGS
)
