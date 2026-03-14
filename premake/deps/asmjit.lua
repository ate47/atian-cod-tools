project "asmjit"
    language "C++"
    kind "StaticLib"
    characterset "MBCS"
    warnings "Off"

    targetname "asmjit"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"
    
    files {
        ROOT .. "/deps/asmjit/src/**.cpp",
        ROOT .. "/deps/asmjit/src/**.h",
    }
    defines { 
        "ASMJIT_STATIC",
        "ASMJIT_NO_AARCH64",
        "ASMJIT_BUILD_RELEASE",
        "ASMJIT_NO_FOREIGN"
    }
    
