project "xsk-gsc"
    language "C++"
    cppdialect "C++20"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"
    buildoptions "/bigobj"
    buildoptions "/Zc:__cplusplus"

    targetname "xsk-gsc"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/gsc-tool/include/xsk/gsc/**.h",
        ROOT .. "/deps/gsc-tool/include/xsk/gsc/**.hpp",
        ROOT .. "/deps/gsc-tool/src/gsc/**.cpp"
    }

    defines { 
        "XSK_NO_COMPILED_HASH" 
    }
    
    includedirs {
        ROOT .. "/deps/gsc-tool/include",
    }
    
    dependson { "xsk-utils" }
    links { "xsk-utils" }