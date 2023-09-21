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
    
    # License
    Copy-Item "README.md" "$base/README.md" > $null
    Copy-Item "LICENSE.md" "$base/licenses/license_acts.md" > $null
    Copy-Item "deps/antlr4/LICENSE.txt" "$base/licenses/license_antlr4.md" > $null
    Copy-Item "deps/Detours/LICENSE.md" "$base/licenses/license_detours.md" > $null

    # Compress
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base.zip" > $null
}
finally {
    $prevPwd | Set-Location
}