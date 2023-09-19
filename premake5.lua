
workspace "AtianCodTools"
    startproject "AtianCodTools"
    location "./build"
    configurations { 
        "Debug", 
        "Release" 
    }

    architecture "x86_64"
    platforms "x64"

    
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
    }

    vpaths {
        ["*"] = "*"
    }

project "ACTS-BO4-DLL"
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
    }

    vpaths {
        ["*"] = "*"
    }
    
    links { "ACTSSharedLibrary" }
    dependson "ACTSSharedLibrary"

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
        "./resources/**",
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
		"deps/antlr4/runtime/Cpp/runtime/src/"
    }

    vpaths {
        ["*"] = "*"
    }

    defines { 
        "ANTLR4CPP_STATIC"
    }
    
    links { "antlr4-runtime" }
    links { "ACTSSharedLibrary" }
    dependson "antlr4-runtime"
    dependson "ACTSSharedLibrary"

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
