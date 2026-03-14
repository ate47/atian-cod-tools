project "mstch"
    language "C++"
    kind "StaticLib"
    cppdialect "C++20"
    characterset "MBCS"

    targetname "mstch"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/mstch/src/**.cpp",
        ROOT .. "/deps/mstch/src/**.hpp",
        ROOT .. "/deps/mstch/include/**.hpp"
    }

    includedirs {
        ROOT .. "/deps/mstch/include/",
        ROOT .. "/deps/mstch/src/"
    }