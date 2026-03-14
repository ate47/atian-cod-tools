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
        ROOT .. "/deps/antlr4/runtime/Cpp/runtime/src/**.hpp",
        ROOT .. "/deps/antlr4/runtime/Cpp/runtime/src/**.h",
        ROOT .. "/deps/antlr4/runtime/Cpp/runtime/src/**.cpp",
    }

    includedirs {
        ROOT .. "/deps/antlr4/runtime/Cpp/runtime/src/"
    }
    
    defines { 
        --"ANTLR4CPP_DLL", -- for SharedLib kind
        "ANTLR4CPP_STATIC"
    }