project "imgui"
    language "C++"
    kind "StaticLib"
    cppdialect "C++17"
    characterset "MBCS"
    warnings "Off"

    targetname "imgui"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/imgui/*.cpp",
        ROOT .. "/deps/imgui/*.h",
        ROOT .. "/deps/imgui/backends/imgui_impl_dx*.cpp",
        ROOT .. "/deps/imgui/backends/imgui_impl_dx*.h",
        ROOT .. "/deps/imgui/backends/imgui_impl_glfw.cpp",
        ROOT .. "/deps/imgui/backends/imgui_impl_glfw.h",
        ROOT .. "/deps/imgui/backends/imgui_impl_opengl2.cpp",
        ROOT .. "/deps/imgui/backends/imgui_impl_opengl2.h",
        ROOT .. "/deps/imgui/backends/imgui_impl_win32*.cpp",
        ROOT .. "/deps/imgui/backends/imgui_impl_win32*.h",
        ROOT .. "/deps/imgui/misc/cpp/imgui_stdlib.*"
    }

    defines {
    }

    links { "glfw" }
    dependson "glfw"
    includedirs {
        ROOT .. "/deps/glfw/include/",
        ROOT .. "/deps/imgui/"
    }

