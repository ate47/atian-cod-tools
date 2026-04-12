param(
    $outDirectory
)


# Delete previous builds
Remove-Item -Recurse -Force -ErrorAction Ignore  "$outDirectory" > $null
New-Item "$outDirectory" -ItemType Directory > $null
$outDirectory = (Get-Item $outDirectory).FullName

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    
    # Create structure
    New-Item "$outDirectory/gpl3" -ItemType Directory > $null
    New-Item "$outDirectory/all" -ItemType Directory > $null
    # acts licenses
    Copy-Item "LICENSE.md" "$outDirectory/licenses.md" > $null

    # common licenses
    Copy-Item "licenses/mit.md" "$outDirectory/mit.md" > $null
    Copy-Item "licenses/gpl3.md" "$outDirectory/gpl3.md" > $null
    Copy-Item "licenses/lgpl3.md" "$outDirectory/lgpl3.md" > $null

    # licenses
    Copy-Item "deps/antlr4/LICENSE.txt" "$outDirectory/all/antlr4.txt" > $null
    Copy-Item "deps/Detours/LICENSE.md" "$outDirectory/all/detours.md" > $null
    Copy-Item "deps/zlib/LICENSE" "$outDirectory/all/zlib.txt" > $null
    Copy-Item "deps/asmjit/LICENSE.md" "$outDirectory/all/asmjit.md" > $null
    Copy-Item "deps/casclib/LICENSE" "$outDirectory/all/casclib.txt" > $null
    Copy-Item "deps/lz4/lib/LICENSE" "$outDirectory/all/lz4.txt" > $null
    Copy-Item "licenses/mio.md" "$outDirectory/all/mio.md" > $null
    Copy-Item "licenses/xorstr.md" "$outDirectory/all/xorstr.md" > $null
    Copy-Item "deps/imgui/LICENSE.txt" "$outDirectory/all/imgui.txt" > $null
    Copy-Item "deps/hw_break/LICENSE" "$outDirectory/all/hw_break.txt" > $null
    Copy-Item "deps/rapidjson/license.txt" "$outDirectory/all/rapidjson.txt" > $null
    Copy-Item "deps/rapidcsv/LICENSE" "$outDirectory/all/rapidcsv.txt" > $null
    Copy-Item "deps/curl/COPYING" "$outDirectory/all/curl.txt" > $null
    Copy-Item "deps/glfw/LICENSE.md" "$outDirectory/all/glfw.md" > $null
    Copy-Item "deps/thread-pool/LICENSE.txt" "$outDirectory/all/thread-pool.txt" > $null
    Copy-Item "deps/stb/LICENSE" "$outDirectory/all/stb.txt" > $null
    Copy-Item "deps/tomlplusplus/LICENSE" "$outDirectory/all/tomlplusplus.txt" > $null
    Copy-Item "deps/miniz-cpp/LICENSE.md" "$outDirectory/all/miniz-cpp.md" > $null
    Copy-Item "deps/zstd/LICENSE" "$outDirectory/all/zstd.md" > $null
    Copy-Item "deps/hash-library/LICENSE" "$outDirectory/all/hash-library.md" > $null
    Copy-Item "deps/hksc/COPYRIGHT" "$outDirectory/all/hksc.md" > $null
    Copy-Item "deps/crc_cpp/LICENSE" "$outDirectory/all/crc_cpp.txt" > $null
    Copy-Item "deps/xxHash/LICENSE" "$outDirectory/all/xxHash.txt" > $null
    Copy-Item "deps/libtommath/LICENSE" "$outDirectory/all/libtommath.txt" > $null
    Copy-Item "deps/libtomcrypt/LICENSE" "$outDirectory/all/libtomcrypt.txt" > $null
    Copy-Item "deps/gsc-tool/LICENSE" "$outDirectory/gpl3/gsc-tool.txt" > $null
}
finally {
    $prevPwd | Set-Location
}