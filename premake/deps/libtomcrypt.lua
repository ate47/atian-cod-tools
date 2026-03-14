project "libtomcrypt"
    language "C"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"

    targetname "libtomcrypt"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/libtomcrypt/src/**.c",
        ROOT .. "/deps/libtomcrypt/src/**.h"
    }

    defines { 
        "LTM_DESC"  
    }

    includedirs {
        ROOT .. "/deps/libtomcrypt/src/headers/",
        ROOT .. "/deps/libtommath/"
    }
    links { "libtommath" }
    dependson "libtommath"
