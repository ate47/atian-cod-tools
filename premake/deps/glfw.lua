project "glfw"
    language "C"
    kind "StaticLib"
    warnings "Off"
    characterset "MBCS"

    targetname "glfw"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    defines {
        "_GLFW_WIN32",
        "_CRT_SECURE_NO_WARNINGS"
    }

    files {
        ROOT .. "/deps/glfw/include/GLFW/glfw3.h",
        ROOT .. "/deps/glfw/include/GLFW/glfw3native.h",
        ROOT .. "/deps/glfw/src/internal.h",
        ROOT .. "/deps/glfw/src/platform.h",
        ROOT .. "/deps/glfw/src/mappings.h",
        ROOT .. "/deps/glfw/src/context.c",
        ROOT .. "/deps/glfw/src/init.c",
        ROOT .. "/deps/glfw/src/input.c",
        ROOT .. "/deps/glfw/src/monitor.c",
        ROOT .. "/deps/glfw/src/platform.c",
        ROOT .. "/deps/glfw/src/vulkan.c",
        ROOT .. "/deps/glfw/src/window.c",
        ROOT .. "/deps/glfw/src/egl_context.c",
        ROOT .. "/deps/glfw/src/osmesa_context.c",
        ROOT .. "/deps/glfw/src/null_platform.h",
        ROOT .. "/deps/glfw/src/null_joystick.h",
        ROOT .. "/deps/glfw/src/null_init.c",

        ROOT .. "/deps/glfw/src/null_monitor.c",
        ROOT .. "/deps/glfw/src/null_window.c",
        ROOT .. "/deps/glfw/src/null_joystick.c",

        ROOT .. "/deps/glfw/src/win32_init.c",
        ROOT .. "/deps/glfw/src/win32_module.c",
        ROOT .. "/deps/glfw/src/win32_joystick.c",
        ROOT .. "/deps/glfw/src/win32_monitor.c",
        ROOT .. "/deps/glfw/src/win32_time.h",
        ROOT .. "/deps/glfw/src/win32_time.c",
        ROOT .. "/deps/glfw/src/win32_thread.h",
        ROOT .. "/deps/glfw/src/win32_thread.c",
        ROOT .. "/deps/glfw/src/win32_window.c",
        ROOT .. "/deps/glfw/src/wgl_context.c",
        ROOT .. "/deps/glfw/src/egl_context.c",
        ROOT .. "/deps/glfw/src/osmesa_context.c"
    }

    includedirs {
        ROOT .. "/deps/glfw/src",
        ROOT .. "/deps/glfw/include"
    }
