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
 
    # Binaries
    Copy-Item "build/bin/*.exe" "$base/bin" > $null
    Copy-Item "build/bin/*.dll" "$base/bin" > $null
    
    # Clear test dlls
    Remove-Item -Force -ErrorAction Ignore "$base/bin/test-*.dll" > $null
    # remove UI (not ready for deployment)
    Remove-Item -Force -ErrorAction Ignore "$base/bin/acts-ui.exe" > $null

    # License
    Copy-Item "README.md" "$base/README.md" > $null
    Copy-Item "LICENSE.md" "$base/licenses/license_acts.md" > $null
    Copy-Item "deps/antlr4/LICENSE.txt" "$base/licenses/license_antlr4.md" > $null
    Copy-Item "deps/Detours/LICENSE.md" "$base/licenses/license_detours.md" > $null
    Copy-Item "deps/zlib/LICENSE" "$base/licenses/license_zlib.md" > $null
    Copy-Item "deps/asmjit/LICENSE.md" "$base/licenses/license_asmjit.md" > $null

    # Compress
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base.zip" > $null

    Write-Host "Packaged to '$base.zip'"
}
finally {
    $prevPwd | Set-Location
}