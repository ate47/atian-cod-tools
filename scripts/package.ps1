param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    scripts\copydata.ps1

    $base = "build/package/acts"

    # Delete previous builds
    Remove-Item -Recurse -Force -ErrorAction Ignore  "$base" > $null
    Remove-Item -Force -ErrorAction Ignore "$base.zip" > $null
    Remove-Item -Force -ErrorAction Ignore "$base-dev.zip" > $null

    # Create structure
    New-Item "$base" -ItemType Directory > $null
    New-Item "$base/licenses" -ItemType Directory > $null
    New-Item "$base/licenses/gpl3" -ItemType Directory > $null
    New-Item "$base/licenses/all" -ItemType Directory > $null
    New-Item "$base/bin" -ItemType Directory > $null
    New-Item "$base/bin/package_index" -ItemType Directory > $null
    New-Item "$base/bin/deps" -ItemType Directory > $null

    # Build hashes
    Write-Host "Building hash index directory"
    foreach ($file in (Get-ChildItem config\common_hashes)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $fileOut = "$base/bin/package_index/$($file.Name.SubString(0, $split)).wni"
        } else {
            $fileOut = "$base/bin/package_index/$($file.Name).wni"
        }

        build\bin\acts.exe --noUpdater -t wni_gen $file.FullName $fileOut
    }
    Write-Host "Building gscbin string index"
    foreach ($file in (Get-ChildItem config\gscbin\opaques)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $vmName = $file.Name.SubString(0, $split)
        } else {
            $vmName = $file.Name;
        }
        $fileOut = "$base/bin/package_index/gscbin-opaques-$vmName.acef"

        build\bin\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut opaque zstd_hc
    }

    foreach ($file in (Get-ChildItem config\gscbin\functions)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $vmName = $file.Name.SubString(0, $split)
        } else {
            $vmName = $file.Name;
        }
        $fileOut = "$base/bin/package_index/gscbin-functions-$vmName.acef"

        build\bin\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut function zstd_hc
    }
    
    foreach ($file in (Get-ChildItem config\gscbin\methods)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $vmName = $file.Name.SubString(0, $split)
        } else {
            $vmName = $file.Name;
        }
        $fileOut = "$base/bin/package_index/gscbin-methods-$vmName.acef"

        build\bin\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut method zstd_hc
    }
 
    # Binaries
    Copy-Item "build/bin/*.exe" "$base/bin" > $null
    Copy-Item "build/bin/*.dll" "$base/bin" > $null

    # Scans
    Copy-Item "build/bin/data" "$base/bin" -Recurse > $null
    
    # Clear test dlls
    Remove-Item -Force -ErrorAction Ignore "$base/bin/test-*.dll" > $null
    # Clear config data
    # Remove-Item build\bin\acts.json

    # Info data
    Copy-Item "README.md" "$base/README.md" > $null
    Copy-Item "release/version" "$base/bin/version" > $null
    # acts licenses
    Copy-Item "LICENSE.md" "$base/licenses.md" > $null
    Copy-Item "licenses/mit.md" "$base/licenses/mit.md" > $null
    Copy-Item "licenses/gpl3.md" "$base/licenses/gpl3.md" > $null
    # licenses
    Copy-Item "deps/antlr4/LICENSE.txt" "$base/licenses/all/antlr4.txt" > $null
    Copy-Item "deps/Detours/LICENSE.md" "$base/licenses/all/detours.md" > $null
    Copy-Item "deps/zlib/LICENSE" "$base/licenses/all/zlib.txt" > $null
    Copy-Item "deps/asmjit/LICENSE.md" "$base/licenses/all/asmjit.md" > $null
    Copy-Item "deps/casclib/LICENSE" "$base/licenses/all/casclib.txt" > $null
    Copy-Item "deps/lz4/lib/LICENSE" "$base/licenses/all/lz4.txt" > $null
    Copy-Item "licenses/mio.md" "$base/licenses/all/mio.md" > $null
    Copy-Item "licenses/xorstr.md" "$base/licenses/all/xorstr.md" > $null
    Copy-Item "deps/imgui/LICENSE.txt" "$base/licenses/all/imgui.txt" > $null
    Copy-Item "deps/hw_break/LICENSE" "$base/licenses/all/hw_break.txt" > $null
    Copy-Item "deps/rapidjson/license.txt" "$base/licenses/all/rapidjson.txt" > $null
    Copy-Item "deps/rapidcsv/LICENSE" "$base/licenses/all/rapidcsv.txt" > $null
    Copy-Item "deps/curl/COPYING" "$base/licenses/all/curl.txt" > $null
    Copy-Item "deps/glfw/LICENSE.md" "$base/licenses/all/glfw.md" > $null
    Copy-Item "deps/thread-pool/LICENSE.txt" "$base/licenses/all/thread-pool.txt" > $null
    Copy-Item "deps/stb/LICENSE" "$base/licenses/all/stb.txt" > $null
    Copy-Item "deps/tomlplusplus/LICENSE" "$base/licenses/all/tomlplusplus.txt" > $null
    Copy-Item "deps/miniz-cpp/LICENSE.md" "$base/licenses/all/miniz-cpp.md" > $null
    Copy-Item "deps/zstd/LICENSE" "$base/licenses/all/zstd.md" > $null
    Copy-Item "deps/hash-library/LICENSE" "$base/licenses/all/hash-library.md" > $null
    Copy-Item "deps/hksc/COPYRIGHT" "$base/licenses/all/hksc.md" > $null
    Copy-Item "deps/crc_cpp/LICENSE" "$base/licenses/all/crc_cpp.txt" > $null
    Copy-Item "deps/xxHash/LICENSE" "$base/licenses/all/xxHash.txt" > $null
    Copy-Item "deps/libtommath/LICENSE" "$base/licenses/all/libtommath.txt" > $null
    Copy-Item "deps/libtomcrypt/LICENSE" "$base/licenses/all/libtomcrypt.txt" > $null
    Copy-Item "deps/gsc-tool/LICENSE" "$base/licenses/gpl3/gsc-tool.txt" > $null

    "Put the game dependencies in this directory." > "$base/bin/deps/README.md"

    # Compress
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base.zip" > $null
    Write-Host "Packaged to '$base.zip'"

    Write-Host "Building dev package..."
    foreach ($fileItem in (Get-ChildItem $base/bin)) {
        $fileName = $fileItem.Name
        $split = $fileName.LastIndexOf(".exe")

        if ($split -ne -1) {
            $pdbFile = "build/bin/$($fileName.SubString(0, $split)).pdb"
        }
        else {
            $split = $fileName.LastIndexOf(".dll")

            if ($split -ne -1) {
                $pdbFile = "build/bin/$($fileName.SubString(0, $split)).pdb"
            } else {
                continue
            }
        }
        Copy-Item $pdbFile "$base/bin" -ErrorAction Ignore
    }
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base-dev.zip" > $null
    Write-Host "Packaged to '$base-dev.zip'"

}
finally {
    $prevPwd | Set-Location
}