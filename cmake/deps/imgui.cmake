# imgui
file(GLOB IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/deps/imgui/*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/*.h"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_dx*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_dx*.h"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.h"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_opengl2.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_opengl2.h"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_win32*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/backends/imgui_impl_win32*.h"
    "${CMAKE_SOURCE_DIR}/deps/imgui/misc/cpp/imgui_stdlib.*"
)
add_library(imgui STATIC)
target_sources(imgui PRIVATE ${IMGUI_SOURCES})
source_group(
    TREE "${CMAKE_SOURCE_DIR}/deps/imgui"
    FILES ${IMGUI_SOURCES}
)
target_compile_options(imgui PRIVATE /W0)
set_target_properties(imgui PROPERTIES
    OUTPUT_NAME "imgui"
    FOLDER "Dependencies"
)

target_include_directories(imgui PRIVATE
    "${CMAKE_SOURCE_DIR}/deps/glfw/include"
    "${CMAKE_SOURCE_DIR}/deps/imgui"
)
# it was using cpp17 in the premake cfg
append_common_defs(imgui)

target_link_libraries(imgui PRIVATE
    glfw
)

add_dependencies(imgui
    glfw
)
