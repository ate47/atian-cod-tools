project "libps4debug"
    language "C++"
    kind "StaticLib"
    cppdialect "C++17"
    characterset "MBCS"
    warnings "Off"

    targetname "libps4debug"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/ps4debug/libdebug/cpp/source/*.cpp",
        ROOT .. "/deps/ps4debug/libdebug/cpp/include/*.hpp"
    }

    includedirs {
        ROOT .. "/deps/ps4debug/libdebug/cpp/include/"
    }
    
