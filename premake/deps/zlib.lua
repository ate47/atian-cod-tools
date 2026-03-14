project "zlib"
    language "C"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"

    targetname "zlib"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    
    files {
        ROOT .. "/deps/zlib/*.c",
        ROOT .. "/deps/zlib/*.h",
    }
