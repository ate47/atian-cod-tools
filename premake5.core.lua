group "Core"
    project "ACTSSharedLibrary"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        pchheader "includes_shared.hpp"
        pchsource "./src/core/shared/includes_shared.cpp"

        targetname "actsshared"
        
        files {
            "./src/core/shared/**.hpp",
            "./src/core/shared/**.h",
            "./src/core/shared/**.cpp",
        }

        includedirs {
            "src/core/shared",
            "deps/asmjit/src/",
            "deps/Detours/src/",
            "deps/curl/include/",
            "deps/rapidjson/include/",
            "deps/miniz-cpp/",
            "deps/dbflib/src/lib/",
            "deps/lz4/lib/",
            "deps/zstd/lib/",
            "deps/zlib/",
            "deps/ps4debug/kdebugger/include/",
            "deps/ps4debug/ps4-ksdk/include/",
            "deps/crc_cpp/include/",
        }

        vpaths {
            ["*"] = "*"
        }
        links { "asmjit" }
        links { "detours" }
        links { "libcurl" }
        links { "lz4" }
        links { "zstd" }
        links { "zlib" }
        dependson "detours"
        dependson "asmjit"
        dependson "libcurl"
        dependson "lz4"
        dependson "zstd"
        dependson "zlib"

    project "ACTSLibrary"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "actslib"
        
        files {
            "./src/core/lib/**.hpp",
            "./src/core/lib/**.h",
            "./src/core/lib/**.cpp",
        }

        includedirs {
            "src/core/actslib",
        }

        vpaths {
            ["*"] = "*"
        }


    project "ACTSVM"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        targetname "acts-vm"
        
        files {
            "./src/core/vm/**.hpp",
            "./src/core/vm/**.h",
            "./src/core/vm/**.cpp",
        }

        includedirs {
            "src/core/shared"
        }

        vpaths {
            ["*"] = "*"
        }
        
        links { "ACTSSharedLibrary" }
        dependson "ACTSSharedLibrary"

    project "AtianCodTools"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        targetdir "%{wks.location}/bin/"
        objdir "%{wks.location}/obj/"

        pchheader "includes.hpp"
        pchsource "./src/core/acts/includes.cpp"

        targetname "acts-common"
        
        files {
            "./.github/workflows/**",
            "./src/core/acts/**.hpp",
            "./src/core/acts/**.h",
            "./src/core/acts/**.cpp",
            "./gsc/**.gsc",
            "./gsc/**.csc",
            "./grammar/**.g4",
            "./scripts/**.ps1",
            "./release/**",
            "./resources/common/**",
            "./config/**",
            ".gitignore",
            "premake5.lua",
            "packages.txt",
            "gsc.conf",
            "LICENSE",
            "README.md"
        }

        includedirs {
            "src/core/acts",
            "src/core/shared",
            "src/core/lib",
            "src/core/vm",
        -- link antlr4
            "deps/antlr4/runtime/Cpp/runtime/src/",
            "deps/zlib/",
            "deps/ps4debug/libdebug/cpp/include/",
            "deps/ps4debug/kdebugger/include/",
            "deps/ps4debug/ps4-ksdk/include/",
            "deps/asmjit/src/",
            "deps/curl/include/",
            "deps/casclib/src/",
            "deps/lz4/lib/",
            "deps/zstd/lib/",
            "deps/salsa20/Source/",
            "deps/Detours/src/",
            "deps/rapidjson/include/",
            "deps/dbflib/src/lib/",
            "deps/json-rpc-cxx/include/",
            "deps/json/include/",
            "deps/rapidcsv/src/",
            "deps/hw_break/HwBpLib/inc/",
            "deps/glfw/include/",
            "deps/imgui/",
            "deps/thread-pool/include/",
            "deps/stb/",
            "deps/tomlplusplus/include/toml++/",
            "deps/miniz-cpp/",
            "deps/hash-library/",
            "deps/hksc/etc/",
            "deps/hksc/src/",
            "deps/crc_cpp/include/",
            "deps/libtomcrypt/src/headers/",
            "deps/mstch/include/",
        }

        vpaths {
            ["*"] = "*"
        }

        defines { 
            "ANTLR4CPP_STATIC",
            "CASCLIB_NO_AUTO_LINK_LIBRARY",
            "ACTS_COMMON_DLL",
            "LTM_DESC",
        }
        
        links { "antlr4-runtime" }
        links { "ACTSSharedLibrary" }
        links { "ACTSLibrary" }
        links { "ACTSVM" }
        links { "zlib" }
        links { "libps4debug" }
        links { "asmjit" }
        links { "casclib" }
        links { "lz4" }
        links { "zstd" }
        links { "detours" }
        links { "libcurl" }
        links { "glfw" }
        links { "OpenGL32" }
        links { "GLU32" }
        links { "imgui" }
        links { "Crypt32.lib" }
        links { "hashlibrary" }
        links { "xxhash" }
        links { "hksc" }
        links { "libtomcrypt" }
        links { "mstch" }
        dependson "detours"
        dependson "antlr4-runtime"
        dependson "ACTSSharedLibrary"
        dependson "ACTSLibrary"
        dependson "ACTSVM"
        dependson "zlib"
        dependson "libps4debug"
        dependson "asmjit"
        dependson "casclib"
        dependson "lz4"
        dependson "zstd"
        dependson "glfw"
        dependson "imgui"
        dependson "hashlibrary"
        dependson "xxhash"
        dependson "hksc"
        dependson "libtomcrypt"
        dependson "mstch"
        
        filter { "options:gpl-build" }
            links { "xsk-arc" }
            links { "xsk-gsc" }
            dependson "xsk-arc"
            dependson "xsk-gsc"
            includedirs { "deps/gsc-tool/include/" }
