newoption {
   trigger = "ci-build",
   description = "CI build define"
}

function buildinfo()

    local versionfile = assert(io.open("release/version", "r"))
    local version = assert(versionfile:read())
    local versionid = assert(versionfile:read())
    versionfile:close()

    -- set version
    local file = assert(io.open("src/shared/actsinfo.hpp", "w"))
    file:write("#pragma once\n")
    file:write("namespace actsinfo {\n")
    file:write("    // Do not write in this file, it is updated by premake\n")
    file:write("#ifdef DEBUG\n\n")
    file:write("    constexpr const char* VERSION = \"Dev\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"Dev\";\n")
    file:write("    constexpr unsigned int VERSION_ID = 0xFFFFFFFF;\n")
    file:write("\n#else\n\n")
    file:write("    // Version used for the release\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "\";\n")
    file:write("    constexpr unsigned int VERSION_ID = 0x" .. versionid .. ";\n")
    file:write("\n#endif\n")
    file:write("}\n")
    file:close()

end

workspace "AtianCodTools"
    startproject "AtianCodTools"
    location "./build"
    configurations { 
        "Debug",
        "Release"
    }

    architecture "x86_64"
    platforms "x64"

    buildinfo()

    filter { "options:ci-build" }
        defines { "CI_BUILD" }
    
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
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "actsshared"
    
    files {
        "./src/shared/**.hpp",
        "./src/shared/**.h",
        "./src/shared/**.cpp",
    }

    includedirs {
        "src/shared",
        "deps/asmjit/src/",
    }

    vpaths {
        ["*"] = "*"
    }
    links { "asmjit" }
    dependson "asmjit"

project "AtianCodToolsBO4DLL"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts-bo4-dll"
    
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
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    links { "detours" }
    links { "asmjit" }
    dependson "ACTSSharedLibrary"
    dependson "detours"
    dependson "asmjit"

project "AtianCodToolsUI"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    targetname "acts-ui"
    
    files {
        "./src/ui/**.hpp",
        "./src/ui/**.cpp",
        "./resources/**",
    }

    includedirs {
        "src/ui",
        "src/shared",
        "deps/ps4debug/libdebug/cpp/include/",
        "deps/asmjit/src/",
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    links { "libps4debug" }
    links { "asmjit" }
    dependson "ACTSSharedLibrary"
    dependson "libps4debug"
    dependson "asmjit"

project "AtianCodTools"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts"
    
    files {
        "./.github/workflows/**",
        "./src/cli/**.hpp",
        "./src/cli/**.h",
        "./src/cli/**.cpp",
        "./gsc/**.gsc",
        "./gsc/**.csc",
        "./grammar/**.g4",
        "./scripts/**.ps1",
        "./release/**",
        "./resources/**",
        "./config/**",
        ".gitignore",
        "premake5.lua",
        "packages.txt",
        "gsc.conf",
        "LICENSE",
        "README.md"
    }

    includedirs {
        "src/cli",
        "src/shared",
    -- link antlr4
		"deps/antlr4/runtime/Cpp/runtime/src/",
		"deps/zlib/",
        "deps/ps4debug/libdebug/cpp/include/",
        "deps/asmjit/src/",
    }

    vpaths {
        ["*"] = "*"
    }

    defines { 
        "ANTLR4CPP_STATIC"
    }
    
    links { "antlr4-runtime" }
    links { "ACTSSharedLibrary" }
    links { "zlib" }
    links { "libps4debug" }
    links { "asmjit" }
    dependson "antlr4-runtime"
    dependson "ACTSSharedLibrary"
    dependson "zlib"
    dependson "libps4debug"
    dependson "asmjit"

project "TestDll"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
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
    }

    vpaths {
        ["*"] = "*"
    }

group "deps"
    project "antlr4-runtime"
        language "C++"
        kind "StaticLib"
        cppdialect "C++20"
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

