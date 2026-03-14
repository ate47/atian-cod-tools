project "zstd"
    language "C"
    kind "StaticLib"
    characterset "MBCS"

    targetname "zstd"
    targetdir "%{wks.location}/bin/"
    objdir "%{wks.location}/obj/"

    files {
        ROOT .. "/deps/zstd/lib/zstd.h",
        ROOT .. "/deps/zstd/lib/common/**.c",
        ROOT .. "/deps/zstd/lib/common/**.h",
        ROOT .. "/deps/zstd/lib/decompress/**.c",
        ROOT .. "/deps/zstd/lib/decompress/**.h",
        ROOT .. "/deps/zstd/lib/compress/**.c",
        ROOT .. "/deps/zstd/lib/compress/**.h",
    }

    defines {
        "XXH_NAMESPACE=ZSTD_",
    }

    includedirs {
        ROOT .. "/deps/zstd/lib/",
        ROOT .. "/deps/zstd/lib/common",
    }
    dependson { "lz4" }
    dependson { "zlib" }
    links { "lz4" }
    links { "zlib" }

