project "hksc"
    language "C"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"

    targetname "hksc"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/hksc/src/*.c",
        ROOT .. "/deps/hksc/src/*.def",
        ROOT .. "/deps/hksc/src/*.h",
        ROOT .. "/deps/hksc/src/*.hpp"
    }

    includedirs {
        ROOT .. "/deps/hksc/src/"
    }

