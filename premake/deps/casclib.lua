project "casclib"
    language "C++"
    kind "StaticLib"
    cppdialect "C++17"
    characterset "MBCS"
    warnings "Off"

    targetname "CascLib"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/casclib/src/**.cpp",
        ROOT .. "/deps/casclib/src/**.h",
        ROOT .. "/deps/casclib/src/**.c",
        ROOT .. "/deps/casclib/src/*.def",
        ROOT .. "/deps/casclib/src/*.rc"
    }
    removefiles {
        ROOT .. "/deps/casclib/src/overwatch/cmf-key.cpp"
    }

    defines {
        "_LIB",
        "_7ZIP_ST",
        "BZ_STRICT_ANSI",
        "CASCLIB_NO_AUTO_LINK_LIBRARY"
    }

    includedirs {
        ROOT .. "/deps/casclib/src/"
    }

