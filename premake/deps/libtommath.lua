project "libtommath"
    language "C"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"

    targetname "libtommath"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/libtommath/*.c",
        ROOT .. "/deps/libtommath/*.h",
        ROOT .. "/deps/libtommath/*.def"
    }

    includedirs {
        ROOT .. "/deps/libtommath/"
    }