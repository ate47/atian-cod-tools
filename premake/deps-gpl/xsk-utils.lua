project "xsk-utils"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"

    targetname "xsk-utils"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/gsc-tool/include/xsk/utils/**.h",
        ROOT .. "/deps/gsc-tool/include/xsk/utils/**.hpp",
        ROOT .. "/deps/gsc-tool/src/utils/**.cpp"
    }

    includedirs {
        ROOT .. "/deps/gsc-tool/include",
        ROOT .. "/deps/zlib/",
    }

    dependson { "zlib" }
    links { "zlib" }