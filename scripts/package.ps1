param(
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    

    $base = "build/package/acts"

    # Delete previous builds
    Remove-Item -Recurse -Force -ErrorAction Ignore  "$base" > $null
    Remove-Item -Force -ErrorAction Ignore "$base.zip" > $null

    # Create structure
    New-Item "$base" -ItemType Directory > $null
    New-Item "$base/licenses" -ItemType Directory > $null
    New-Item "$base/bin" -ItemType Directory > $null
    New-Item "$base/bin/package_index" -ItemType Directory > $null

    # Build hashes
    Write-Host "Building hash index directory"
    foreach ($file in (Get-ChildItem config\common_hashes)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $fileOut = "$base/bin/package_index/$($file.Name.SubString(0, $split)).wni"
        } else {
            $fileOut = "$base/bin/package_index/$($file.Name).wni"
        }

        build\bin\acts.exe wni_gen $file.FullName $fileOut
    }

    # Build default acpf
    build\bin\acts packfile "$base/bin/package_index/common.acpf"
 
    # Binaries
    Copy-Item "build/bin/*.exe" "$base/bin" > $null
    Copy-Item "build/bin/*.dll" "$base/bin" > $null
    
    # Clear test dlls
    Remove-Item -Force -ErrorAction Ignore "$base/bin/test-*.dll" > $null

    # License
    Copy-Item "README.md" "$base/README.md" > $null
    Copy-Item "LICENSE.md" "$base/licenses/acts.md" > $null
    Copy-Item "deps/antlr4/LICENSE.txt" "$base/licenses/antlr4.txt" > $null
    Copy-Item "deps/Detours/LICENSE.md" "$base/licenses/detours.md" > $null
    Copy-Item "deps/zlib/LICENSE" "$base/licenses/zlib.txt" > $null
    Copy-Item "deps/asmjit/LICENSE.md" "$base/licenses/asmjit.md" > $null
    Copy-Item "deps/casclib/LICENSE" "$base/licenses/casclib.txt" > $null
    Copy-Item "deps/lz4/lib/LICENSE" "$base/licenses/lz4.txt" > $null
    Copy-Item "licenses/mio.md" "$base/licenses/mio.md" > $null
    Copy-Item "licenses/xorstr.md" "$base/licenses/xorstr.md" > $null
    Copy-Item "deps/imgui/LICENSE.txt" "$base/licenses/imgui.txt" > $null
    Copy-Item "deps/hw_break/LICENSE" "$base/licenses/hw_break.txt" > $null
    Copy-Item "deps/rapidjson/license.txt" "$base/licenses/rapidjson.txt" > $null

    # Compress
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base.zip" > $null

    Write-Host "Packaged to '$base.zip'"
}
finally {
    $prevPwd | Set-Location
}