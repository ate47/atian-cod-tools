
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
        optimize "On"

project "AtianCodTools"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    targetname "acts"
    
    files {
        "./src/**.hpp",
        "./src/**.cpp",
        ".gitignore",
        "premake5.lua",
        "LICENSE",
        "README.md"
    }

    includedirs {
        "src"
    }

    vpaths {
        ["*"] = "*"
    }
