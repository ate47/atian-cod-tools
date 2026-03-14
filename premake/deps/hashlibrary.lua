project "hashlibrary"
    language "C++"
    kind "StaticLib"
    cppdialect "C++20"
    characterset "MBCS"

    targetname "hashlibrary"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/hash-library/*.cpp",
        ROOT .. "/deps/hash-library/*.h"
    }

    includedirs {
        ROOT .. "/deps/hash-library/"
    }
    

