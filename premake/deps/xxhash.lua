project "xxhash"
    language "C++"
    kind "StaticLib"
    cppdialect "C++20"
    characterset "MBCS"

    targetname "xxhash"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/xxHash/xxhash.c",
        ROOT .. "/deps/xxHash/xxhash.h"
    }

    includedirs {
        ROOT .. "/deps/xxHash/"
    }

