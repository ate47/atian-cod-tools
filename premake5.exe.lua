group "Executables"
    project "AtianCodToolsCLI"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts"
        
        files {
            "./resources/cli/**",
            "./src/exe/cli/**.hpp",
            "./src/exe/cli/**.h",
            "./src/exe/cli/**.cpp"
        }

        includedirs {
            "src/exe/cli",
            "src/core/acts",
        }

        vpaths {
            ["*"] = "*"
        }
        links { "AtianCodTools" }
        dependson "AtianCodTools"
        
    project "AtianCodToolsCordycep"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "Cordycep.CLI"
        
        files {
            "./resources/cordycep/**",
            "./src/exe/cordycep/**.hpp",
            "./src/exe/cordycep/**.h",
            "./src/exe/cordycep/**.cpp"
        }
        includedirs {
            "src/exe/cordycep",
            "src/core/acts",
        }

        vpaths {
            ["*"] = "*"
        }
        links { "AtianCodTools" }
        dependson "AtianCodTools"

    project "AtianCodToolsUI"
        kind "WindowedApp"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        targetname "acts-ui"
        
        files {
            "./src/exe/ui/**.hpp",
            "./src/exe/ui/**.cpp",
            "./resources/ui/**",
        }

        includedirs {
            "src/core/acts",
            "src/exe/ui",
            "src/core/shared",
            "deps/ps4debug/libdebug/cpp/include/",
            "deps/asmjit/src/",
            "deps/curl/include/",
            "deps/Detours/src/",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "AtianCodTools" }
        links { "ACTSSharedLibrary" }
        links { "libps4debug" }
        links { "asmjit" }
        links { "detours" }
        links { "libcurl" }
        dependson "AtianCodTools"
        dependson "detours"
        dependson "ACTSSharedLibrary"
        dependson "libps4debug"
        dependson "asmjit"

        vpaths {
            ["*"] = "*"
        }

    project "AtianCodToolsUpdater"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"
        targetname "acts-updater"
        
        files {
            "./src/exe/updater/**.hpp",
            "./src/exe/updater/**.cpp",
            "./resources/updater/**",
        }

        includedirs {
            "src/exe/updater",
            "src/core/shared",
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        dependson "ACTSSharedLibrary"

        vpaths {
            ["*"] = "*"
        }