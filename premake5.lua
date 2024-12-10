newoption {
   trigger = "ci-build",
   description = "CI build define"
}
newoption {
   trigger = "prerelease-build",
   description = "CI prerelease build define"
}

function buildinfo()
    local versionfile = assert(io.open("release/version", "r"))
    local version = assert(versionfile:read())
    local versionid = assert(versionfile:read())
    versionfile:close()

    -- set version
    local file = assert(io.open("src/shared/core/actsinfo.hpp", "w"))
    file:write("#pragma once\n")
    file:write("namespace core::actsinfo {\n")
    file:write("    // Do not write in this file, it is updated by premake\n")
    file:write("    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;\n")
    file:write("#ifdef CI_BUILD\n\n")
    file:write("    // Version used for the release\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "\";\n")
    file:write("    constexpr unsigned int VERSION_ID = 0x" .. versionid .. ";\n")
    file:write("\n#else\n\n")
    file:write("\n#ifdef PRERELEASE_BUILD\n\n")
    file:write("    // prerelease\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "-pre\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "-pre\";\n")
    file:write("\n#else\n\n")
    file:write("    constexpr const char* VERSION = \"Dev\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"Dev\";\n")
    file:write("\n#endif // PRERELEASE_BUILD\n")
    file:write("    // Disable updater\n")
    file:write("    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;\n")
    file:write("\n#endif // CI_BUILD\n")
    file:write("}\n")
    file:close()
end

function patch_dogshit(original, dest)
    local patch = assert(io.open(original , "r"))
    local file = assert(io.open(dest , "w"))
    file:write(patch:read("a"))
    file:close()
    patch:close()
    
end

workspace "AtianCodTools"
    startproject "AtianCodToolsCLI"
    location "./build"
    configurations { 
        "Debug",
        "Release"
    }

    architecture "x86_64"
    platforms "x64"

    buildinfo()
    patch_dogshit("patch/cordycep/log.h", "deps/cordycep/src/Parasyte/Log.h")
    patch_dogshit("patch/cordycep/log.cpp", "deps/cordycep/src/Parasyte/Log.cpp")

    filter { "options:ci-build" }
        defines { "CI_BUILD" }

    filter { "options:prerelease-build" }
        defines { "PRERELEASE_BUILD" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Size"
        
    filter {} -- Reset filters

project "ACTSSharedLibrary"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    pchheader "includes_shared.hpp"
    pchsource "./src/shared/includes_shared.cpp"

    targetname "actsshared"
    
    files {
        "./src/shared/**.hpp",
        "./src/shared/**.h",
        "./src/shared/**.cpp",
    }

    includedirs {
        "src/shared",
        "deps/asmjit/src/",
		"deps/Detours/src/",
        "deps/curl/include/",
        "deps/rapidjson/include/",
        "deps/miniz-cpp/",
        "deps/dbflib/src/lib/",
    }

    vpaths {
        ["*"] = "*"
    }
    links { "asmjit" }
    links { "detours" }
    links { "libcurl" }
    dependson "detours"
    dependson "asmjit"
    dependson "libcurl"

project "ACTSLibrary"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "actslib"
    
    files {
        "./src/lib/**.hpp",
        "./src/lib/**.h",
        "./src/lib/**.cpp",
    }

    includedirs {
        "src/actslib",
    }

    vpaths {
        ["*"] = "*"
    }

project "AtianCodToolsBO4DLL"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-bo4"
    
    files {
        "./src/bo4-dll/**.hpp",
        "./src/bo4-dll/**.h",
        "./src/bo4-dll/**.cpp",
    }

    includedirs {
        "src/bo4-dll",
        "src/shared",
    -- link detours
		"deps/Detours/src/",
        "deps/asmjit/src/",
        "deps/curl/include/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    links { "libcurl" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"
    

project "AtianCodToolsBO4ShieldPlugin"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-shield-plugin"
    
    files {
        "./src/shield-plugin/**.hpp",
        "./src/shield-plugin/**.h",
        "./src/shield-plugin/**.cpp",
    }

    includedirs {
        "src/shield-plugin",
        "src/shared",
    -- link detours
		"deps/Detours/src/",
        "deps/asmjit/src/",
        "deps/curl/include/",
        "deps/rapidjson/include/",
        "deps/dbflib/src/lib/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    links { "libcurl" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"

project "AtianCodToolsBOCWDLL"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-bocw"
    
    files {
        "./src/bocw-dll/**.hpp",
        "./src/bocw-dll/**.h",
        "./src/bocw-dll/**.cpp",
    }

    includedirs {
        "src/bocw-dll",
        "src/shared",
    -- link detours
		"deps/Detours/src/",
        "deps/asmjit/src/",
        "deps/imgui/",
        "deps/hw_break/HwBpLib/inc/",
        "deps/curl/include/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    links { "imgui" }
    links { "libcurl" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"
    dependson "imgui"

project "ACTSVM"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-vm"
    
    files {
        "./src/vm/**.hpp",
        "./src/vm/**.h",
        "./src/vm/**.cpp",
    }

    includedirs {
        "src/shared"
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    dependson "ACTSSharedLibrary"

project "AtianCodTools"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    pchheader "includes.hpp"
    pchsource "./src/acts/includes.cpp"

    targetname "acts-common"
    
    files {
        "./.github/workflows/**",
        "./src/acts/**.hpp",
        "./src/acts/**.h",
        "./src/acts/**.cpp",
        "./gsc/**.gsc",
        "./gsc/**.csc",
        "./grammar/**.g4",
        "./scripts/**.ps1",
        "./release/**",
        "./resources/common/**",
        "./config/**",
        ".gitignore",
        "premake5.lua",
        "packages.txt",
        "gsc.conf",
        "LICENSE",
        "README.md"
    }

    includedirs {
        "src/acts",
        "src/shared",
        "src/lib",
        "src/vm",
    -- link antlr4
		"deps/antlr4/runtime/Cpp/runtime/src/",
		"deps/zlib/",
        "deps/ps4debug/libdebug/cpp/include/",
        "deps/asmjit/src/",
        "deps/curl/include/",
        "deps/casclib/src/",
        "deps/lz4/lib/",
		"deps/Detours/src/",
        "deps/rapidjson/include/",
        "deps/dbflib/src/lib/",
        "deps/json-rpc-cxx/include/",
        "deps/json/include/",
        "deps/rapidcsv/src/",
        "deps/hw_break/HwBpLib/inc/",
        "deps/glfw/include/",
        "deps/imgui/",
        "deps/thread-pool/include/",
        "deps/stb/",
        "deps/tomlplusplus/include/toml++/",
        "deps/miniz-cpp/"
    }

    vpaths {
        ["*"] = "*"
    }

    defines { 
        "ANTLR4CPP_STATIC",
        "CASCLIB_NO_AUTO_LINK_LIBRARY",
        "ACTS_COMMON_DLL"
    }
    
    links { "antlr4-runtime" }
    links { "ACTSSharedLibrary" }
    links { "ACTSLibrary" }
    links { "ACTSVM" }
    links { "zlib" }
    links { "libps4debug" }
    links { "asmjit" }
    links { "casclib" }
    links { "lz4" }
    links { "detours" }
    links { "libcurl" }
    links { "glfw" }
	links { "OpenGL32" }
	links { "GLU32" }
    links { "imgui" }
    dependson "detours"
    dependson "antlr4-runtime"
    dependson "ACTSSharedLibrary"
    dependson "ACTSLibrary"
    dependson "ACTSVM"
    dependson "zlib"
    dependson "libps4debug"
    dependson "asmjit"
    dependson "casclib"
    dependson "lz4"
    dependson "glfw"
    dependson "imgui"

project "AtianCodToolsCLI"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts"
    
    files {
        "./resources/cli/**",
        "./src/cli/**.hpp",
        "./src/cli/**.h",
        "./src/cli/**.cpp"
    }

    includedirs {
        "src/cli",
        "src/acts",
    }

    vpaths {
        ["*"] = "*"
    }
    links { "AtianCodTools" }
    dependson "AtianCodTools"
    
project "AtianCodToolsGPL"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-gpl"
    
    files {
        "./resources/cli-gpl/**",
        "./src/acts-gpl/**.hpp",
        "./src/acts-gpl/**.h",
        "./src/acts-gpl/**.cpp"
    }
    defines { 
        "ACTS_GPL_SHIT"
    }

    includedirs {
        "src/cli",
        "src/acts",
        "src/acts-gpl",
		"deps/Detours/src/",
        "src/shared",
        "deps/cordycep/src/Parasyte/"
    }

    vpaths {
        ["*"] = "*"
    }
    links { "AtianCodTools" }
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links "cordycep"
    dependson "AtianCodTools"
    dependson { "ACTSSharedLibrary" }
    dependson { "detours" }
    dependson "cordycep"
    

project "AtianCodToolsUI"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    targetname "acts-ui"
    
    files {
        "./src/ui/**.hpp",
        "./src/ui/**.cpp",
        "./resources/ui/**",
    }

    includedirs {
        "src/acts",
        "src/ui",
        "src/shared",
        "deps/ps4debug/libdebug/cpp/include/",
        "deps/asmjit/src/",
        "deps/curl/include/",
		"deps/Detours/src/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "AtianCodTools" }
    links { "ACTSSharedLibrary" }
    links { "libps4debug" }
    links { "asmjit" }
    links { "detours" }
    links { "libcurl" }
    dependson "AtianCodTools"
    dependson "detours"
    dependson "ACTSSharedLibrary"
    dependson "libps4debug"
    dependson "asmjit"

    vpaths {
        ["*"] = "*"
    }

project "AtianCodToolsUpdater"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    targetname "acts-updater"
    
    files {
        "./src/updater/**.hpp",
        "./src/updater/**.cpp",
        "./resources/updater/**",
    }

    includedirs {
        "src/updater",
        "src/shared",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    dependson "ACTSSharedLibrary"

    vpaths {
        ["*"] = "*"
    }
    

project "TestDll"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "test-dll"
    
    files {
        "./src/test-dll/**.hpp",
        "./src/test-dll/**.h",
        "./src/test-dll/**.cpp",
    }

    includedirs {
        "./src/test-dll",
		"deps/Detours/src/",
        "src/shared",
        "deps/asmjit/src/",
        "deps/curl/include/",
        "deps/imgui/",
		"deps/Detours/src/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    links { "imgui" }
    links { "libcurl" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"
    dependson "imgui"

project "AtianCodToolsMW23DLL"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-mw23"
    
    files {
        "./src/mw23-dll/**.hpp",
        "./src/mw23-dll/**.h",
        "./src/mw23-dll/**.cpp",
    }

    includedirs {
        "./src/mw23-dll",
		"deps/Detours/src/",
        "src/shared",
        "deps/asmjit/src/",
        "deps/curl/include/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    links { "libcurl" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"
    
group "deps"
    project "antlr4-runtime"
        language "C++"
        kind "StaticLib"
        cppdialect "C++20"
        characterset "MBCS"
        warnings "Off"

        targetname "antlr4-runtime"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        
        files {
            "deps/antlr4/runtime/Cpp/runtime/src/**.hpp",
            "deps/antlr4/runtime/Cpp/runtime/src/**.h",
            "deps/antlr4/runtime/Cpp/runtime/src/**.cpp",
        }

        includedirs {
            "deps/antlr4/runtime/Cpp/runtime/src/"
        }
        
        defines { 
            --"ANTLR4CPP_DLL", -- for SharedLib kind
            "ANTLR4CPP_STATIC"
        }
    project "detours"
        language "C++"
        kind "StaticLib"
        cppdialect "C++20"
        characterset "MBCS"
        warnings "Off"

        targetname "detours"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        
        files {
            "deps/Detours/src/detours.*",
            "deps/Detours/src/modules.*",
            "deps/Detours/src/disasm.*",
            "deps/Detours/src/image.*",
            "deps/Detours/src/creatwth.*",
            "deps/Detours/src/disolx86.*",
            "deps/Detours/src/disolx64.*",
            "deps/Detours/src/disolia64.*",
            "deps/Detours/src/disolarm.*",
            "deps/Detours/src/disolarm64.*"
        }

        includedirs {
            "deps/Detours/src/"
        }
    project "zlib"
        language "C"
        kind "StaticLib"
        characterset "MBCS"
        warnings "Off"

        targetname "zlib"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        
        files {
            "deps/zlib/*.c",
            "deps/zlib/*.h",
        }
    project "asmjit"
        language "C++"
        kind "StaticLib"
        characterset "MBCS"
        warnings "Off"

        targetname "asmjit"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        
        files {
            "deps/asmjit/src/**.cpp",
            "deps/asmjit/src/**.h",
        }
        defines { 
            "ASMJIT_STATIC",
            "ASMJIT_NO_AARCH64",
            "ASMJIT_BUILD_RELEASE",
            "ASMJIT_NO_FOREIGN"
        }
        
    project "libps4debug"
        language "C++"
        kind "StaticLib"
        cppdialect "C++17"
        characterset "MBCS"
        warnings "Off"

        targetname "libps4debug"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        files {
            "deps/ps4debug/libdebug/cpp/source/*.cpp",
            "deps/ps4debug/libdebug/cpp/include/*.hpp"
        }

        includedirs {
            "deps/ps4debug/libdebug/cpp/include/"
        }
        
    project "lz4"
        language "C++"
        kind "StaticLib"
        cppdialect "C++17"
        characterset "MBCS"
        warnings "Off"

        targetname "lz4"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        files {
            "deps/lz4/lib/lz4.c",
            "deps/lz4/lib/lz4.h"
        }

        includedirs {
            "deps/lz4/lib/"
        }
        
    project "casclib"
        language "C++"
        kind "StaticLib"
        cppdialect "C++17"
        characterset "MBCS"
        warnings "Off"

        targetname "CascLib"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        files {
            "deps/casclib/src/**.cpp",
            "deps/casclib/src/**.h",
            "deps/casclib/src/**.c",
            "deps/casclib/src/*.def",
            "deps/casclib/src/*.rc"
        }
        removefiles {
            "deps/casclib/src/overwatch/cmf-key.cpp"
        }

        defines {
            "_LIB",
            "_7ZIP_ST",
            "BZ_STRICT_ANSI",
            "CASCLIB_NO_AUTO_LINK_LIBRARY"
        }

        includedirs {
            "deps/casclib/src/"
        }

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
            "deps/imgui/*.cpp",
            "deps/imgui/*.h",
            "deps/imgui/backends/imgui_impl_dx*.cpp",
            "deps/imgui/backends/imgui_impl_dx*.h",
            "deps/imgui/backends/imgui_impl_glfw.cpp",
            "deps/imgui/backends/imgui_impl_glfw.h",
            "deps/imgui/backends/imgui_impl_opengl2.cpp",
            "deps/imgui/backends/imgui_impl_opengl2.h",
            "deps/imgui/backends/imgui_impl_win32*.cpp",
            "deps/imgui/backends/imgui_impl_win32*.h",
            "deps/imgui/misc/cpp/imgui_stdlib.*"
        }

        defines {
        }

        links { "glfw" }
        dependson "glfw"
        includedirs {
            "deps/glfw/include/",
            "deps/imgui/"
        }

    project "libcurl"
        language "C"
        kind "StaticLib"
        warnings "Off"
        characterset "MBCS"

        targetname "libcurl"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        files {
            "deps/curl/lib/**.c",
            "deps/curl/lib/**.h",
        }

        defines {
            "CURL_STRICTER",
            "CURL_STATICLIB",
            "CURL_DISABLE_DICT",
            "CURL_DISABLE_FILE",
            "CURL_DISABLE_LDAP",
            "CURL_DISABLE_LDAPS",
            "CURL_DISABLE_FTP",
            "CURL_DISABLE_GOPHER",
            "CURL_DISABLE_IMAP",
            "CURL_DISABLE_MQTT",
            "CURL_DISABLE_POP3",
            "CURL_DISABLE_RTSP",
            "CURL_DISABLE_SMTP",
            "CURL_DISABLE_SMB",
            "CURL_DISABLE_TELNET",
            "CURL_DISABLE_TFTP",
            
			"BUILDING_LIBCURL",

			"USE_SCHANNEL",
			"USE_WINDOWS_SSPI",
			"USE_THREADS_WIN32",
        }

	    links { "Crypt32.lib" }

        includedirs {
            "deps/curl/lib",
            "deps/curl/include"
        }

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
            "deps/glfw/include/GLFW/glfw3.h",
            "deps/glfw/include/GLFW/glfw3native.h",
            "deps/glfw/src/internal.h",
            "deps/glfw/src/platform.h",
            "deps/glfw/src/mappings.h",
            "deps/glfw/src/context.c",
            "deps/glfw/src/init.c",
            "deps/glfw/src/input.c",
            "deps/glfw/src/monitor.c",
            "deps/glfw/src/platform.c",
            "deps/glfw/src/vulkan.c",
            "deps/glfw/src/window.c",
            "deps/glfw/src/egl_context.c",
            "deps/glfw/src/osmesa_context.c",
            "deps/glfw/src/null_platform.h",
            "deps/glfw/src/null_joystick.h",
            "deps/glfw/src/null_init.c",

            "deps/glfw/src/null_monitor.c",
            "deps/glfw/src/null_window.c",
            "deps/glfw/src/null_joystick.c",

            "deps/glfw/src/win32_init.c",
            "deps/glfw/src/win32_module.c",
            "deps/glfw/src/win32_joystick.c",
            "deps/glfw/src/win32_monitor.c",
            "deps/glfw/src/win32_time.h",
            "deps/glfw/src/win32_time.c",
            "deps/glfw/src/win32_thread.h",
            "deps/glfw/src/win32_thread.c",
            "deps/glfw/src/win32_window.c",
            "deps/glfw/src/wgl_context.c",
            "deps/glfw/src/egl_context.c",
            "deps/glfw/src/osmesa_context.c"
        }

        includedirs {
            "deps/glfw/src",
            "deps/glfw/include"
        }
    project "cordycep"
        language "C++"
        kind "StaticLib"
        cppdialect "C++20"
        characterset "MBCS"

        targetname "Cordycep"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        pchheader "pch.h"
        pchsource "./deps/cordycep/src/Parasyte/pch.cpp"

        files {
            "deps/cordycep/src/Parasyte/**.cpp",
            "deps/cordycep/src/Parasyte/**.h"
        }

        includedirs {
            "deps/cordycep/src/Parasyte/",
            "deps/casclib/src/",
            "deps/lz4/lib/",
		    "deps/zlib/",
            "deps/tomlplusplus/include/",
            "deps/nlohmann-json/single_include",
		    "deps/Detours/src/",
            "deps/deps-detour",
        }
        dependson { "casclib" }
        dependson { "lz4" }
        dependson { "zlib" }
        links { "casclib" }
        links { "lz4" }
        links { "zlib" }
