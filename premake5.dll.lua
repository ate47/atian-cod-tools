group "Game DLLs"
    --[[
    project "AtianCodToolsBO4DLL"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-bo4"
        
        files {
            "./src/dll/bo4-dll/**.hpp",
            "./src/dll/bo4-dll/**.h",
            "./src/dll/bo4-dll/**.cpp",
        }

        includedirs {
            "src/dll/bo4-dll",
            "src/core/shared",
        -- link detours
            "deps/Detours/src/",
            "deps/asmjit/src/",
            "deps/curl/include/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"
    ]]

    project "AtianCodToolsBO4ShieldPlugin"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-shield-plugin"
        
        files {
            "./src/dll/shield-plugin/**.hpp",
            "./src/dll/shield-plugin/**.h",
            "./src/dll/shield-plugin/**.cpp",
        }

        includedirs {
            "src/dll/shield-plugin",
            "src/core/shared",
        -- link detours
            "deps/Detours/src/",
            "deps/asmjit/src/",
            "deps/curl/include/",
            "deps/rapidjson/include/",
            "deps/dbflib/src/lib/",
            "deps/rapidcsv/src/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"

    project "AtianCodToolsBOCWDLL"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-bocw"
        
        files {
            "./src/dll/bocw-dll/**.hpp",
            "./src/dll/bocw-dll/**.h",
            "./src/dll/bocw-dll/**.cpp",
        }

        includedirs {
            "src/dll/bocw-dll",
            "src/core/shared",
        -- link detours
            "deps/Detours/src/",
            "deps/asmjit/src/",
            "deps/imgui/",
            "deps/hw_break/HwBpLib/inc/",
            "deps/curl/include/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "imgui" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"
        dependson "imgui"
        
    project "AtianCodToolsCOD2020DLL"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-cod2020"
        
        files {
            "./src/dll/bo2020-dll/**.hpp",
            "./src/dll/bo2020-dll/**.h",
            "./src/dll/bo2020-dll/**.cpp",
        }

        includedirs {
            "src/dll/bo2020-dll",
            "src/core/shared",
        -- link detours
            "deps/Detours/src/",
            "deps/asmjit/src/",
            "deps/curl/include/",
            "deps/rapidjson/include/",
            "deps/dbflib/src/lib/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"

    project "TestDll"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "test-dll"
        
        files {
            "./src/dll/test-dll/**.hpp",
            "./src/dll/test-dll/**.h",
            "./src/dll/test-dll/**.cpp",
        }

        includedirs {
            "./src/dll/test-dll",
            "deps/Detours/src/",
            "src/core/shared",
            "deps/asmjit/src/",
            "deps/curl/include/",
            "deps/imgui/",
            "deps/Detours/src/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "imgui" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"
        dependson "imgui"
    project "AtianCodToolsMW23DLL"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-mw23"
        
        files {
            "./src/dll/mw23-dll/**.hpp",
            "./src/dll/mw23-dll/**.h",
            "./src/dll/mw23-dll/**.cpp",
        }

        includedirs {
            "./src/dll/mw23-dll",
            "deps/Detours/src/",
            "src/core/shared",
            "deps/asmjit/src/",
            "deps/curl/include/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        
        links { "ACTSSharedLibrary" }
        links { "detours" }
        links { "asmjit" }
        links { "libcurl" }
        dependson "ACTSSharedLibrary"
        dependson "detours"
        dependson "asmjit"