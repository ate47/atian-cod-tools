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
        ROOT .. "/deps/lz4/lib/lz4.c",
        ROOT .. "/deps/lz4/lib/lz4.h",
        ROOT .. "/deps/lz4/lib/lz4hc.c",
        ROOT .. "/deps/lz4/lib/lz4hc.h"
    }

    includedirs {
        ROOT .. "/deps/lz4/lib/"
    }
    
