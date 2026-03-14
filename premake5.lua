newoption {
   trigger = "ci-build",
   description = "CI build define"
}
newoption {
   trigger = "gpl-build",
   description = "GPL build define"
}
newoption {
   trigger = "prerelease-build",
   description = "CI prerelease build define"
}

function buildinfo()
    local versionfile = assert(io.open("release/version", "r"))
    local version = assert(versionfile:read())
    local versionid = assert(versionfile:read())
    versionfile:close()

    -- set version
    local file = assert(io.open("src/shared/core/actsinfo.hpp", "w"))
    file:write("#pragma once\n")
    file:write("namespace core::actsinfo {\n")
    file:write("    // Do not write in this file, it is updated by premake\n")
    file:write("    constexpr unsigned int DEV_VERSION_ID = 0xFFFFFFFF;\n")
    file:write("#ifdef CI_BUILD\n\n")
    file:write("    // Version used for the release\n")
    file:write("#ifdef GPL_BUILD\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "-gpl\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "-gpl\";\n")
    file:write("#else\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "\";\n")
    file:write("#endif // GPL_BUILD\n")
    file:write("    constexpr unsigned int VERSION_ID = 0x" .. versionid .. ";\n")
    file:write("\n#else\n\n")
    file:write("\n#ifdef PRERELEASE_BUILD\n\n")
    file:write("    // prerelease\n")
    file:write("#ifdef GPL_BUILD\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "-gpl-pre\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "-gpl-pre\";\n")
    file:write("#else\n")
    file:write("    constexpr const char* VERSION = \"" .. version .. "-pre\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"" .. version .. "-pre\";\n")
    file:write("#endif // GPL_BUILD\n")
    file:write("\n#else\n\n")
    file:write("#ifdef GPL_BUILD\n")
    file:write("    constexpr const char* VERSION = \"Dev-gpl\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"Dev-gpl\";\n")
    file:write("#else\n")
    file:write("    constexpr const char* VERSION = \"Dev\";\n")
    file:write("    constexpr const wchar_t* VERSIONW = L\"Dev\";\n")
    file:write("#endif // GPL_BUILD\n")
    file:write("\n#endif // PRERELEASE_BUILD\n")
    file:write("    // Disable updater\n")
    file:write("    constexpr unsigned int VERSION_ID = DEV_VERSION_ID;\n")
    file:write("\n#endif // CI_BUILD\n")
    file:write("}\n")
    file:close()
    
    -- write gsc-tool version
    local file = assert(io.open("deps/gsc-tool/include/xsk/version.hpp", "w"))
    file:write("#define XSK_VERSION_STR    \"ACTS_" .. version .. "\"\n")
    file:close()
end

function patch_dogshit(original, dest)
    local patch = assert(io.open(original , "r"))
    local file = assert(io.open(dest , "w"))
    file:write(patch:read("a"))
    file:close()
    patch:close()
    
end

workspace "AtianCodTools"
    startproject "AtianCodToolsCLI"
    location "./build"
    configurations { 
        "Debug",
        "Release"
    }

    architecture "x86_64"
    platforms "x64"

    buildinfo()
    --patch_dogshit("patch/cordycep/log.h", "deps/cordycep/src/Parasyte/Log.h")
    --patch_dogshit("patch/cordycep/log.cpp", "deps/cordycep/src/Parasyte/Log.cpp")

    filter { "options:ci-build" }
        defines { "CI_BUILD" }

    filter { "options:gpl-build" }
        defines { "GPL_BUILD" }

    filter { "options:prerelease-build" }
        defines { "PRERELEASE_BUILD" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Size"
        
    filter {} -- Reset filters

ROOT = path.getabsolute(".")

for _, file in ipairs(os.matchfiles("premake5.*.lua")) do
    printf("Including %s", file)
    include(file)
end