project "detours"
    language "C++"
    kind "StaticLib"
    cppdialect "C++20"
    characterset "MBCS"
    warnings "Off"

    targetname "detours"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    
    files {
        ROOT .. "/deps/Detours/src/detours.*",
        ROOT .. "/deps/Detours/src/modules.*",
        ROOT .. "/deps/Detours/src/disasm.*",
        ROOT .. "/deps/Detours/src/image.*",
        ROOT .. "/deps/Detours/src/creatwth.*",
        ROOT .. "/deps/Detours/src/disolx86.*",
        ROOT .. "/deps/Detours/src/disolx64.*",
        ROOT .. "/deps/Detours/src/disolia64.*",
        ROOT .. "/deps/Detours/src/disolarm.*",
        ROOT .. "/deps/Detours/src/disolarm64.*"
    }

    includedirs {
        ROOT .. "/deps/Detours/src/"
    }
