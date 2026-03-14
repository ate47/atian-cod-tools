project "xsk-arc"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"

    targetname "xsk-arc"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/gsc-tool/include/xsk/arc/**.h",
        ROOT .. "/deps/gsc-tool/include/xsk/arc/**.hpp",
        ROOT .. "/deps/gsc-tool/src/arc/**.cpp"
    }

    defines { 
        "XSK_NO_COMPILED_HASH" 
    }

    includedirs {
        ROOT .. "/deps/gsc-tool/include",
    }

    dependson { "xsk-utils" }
    links { "xsk-utils" }