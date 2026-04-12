param(
    [switch]
    $deployQt
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
    Remove-Item -Force -ErrorAction Ignore "$base-library.zip" > $null

    # Create structure
    New-Item "$base" -ItemType Directory > $null
    New-Item "$base/bin" -ItemType Directory > $null
    New-Item "$base/lib" -ItemType Directory > $null


    # api
    Copy-Item "build/bin/Release/acts-common.dll" "$base/bin" > $null
    Copy-Item "build/bin/Release/acts-common.lib" "$base/lib" > $null
    Copy-Item "build/bin/Release/acts-common-static.lib" "$base/lib" -ErrorAction Ignore > $null
    Copy-Item -Recurse "include" "$base" > $null
    Copy-Item -Recurse "licenses" "$base" > $null
    Copy-Item -Recurse "examples" "$base" > $null

    # Scans
    Copy-Item "build/bin/Release/data" "$base/bin" -Recurse > $null
    
    # Clear test dlls/exes
    Remove-Item -Force -ErrorAction Ignore "$base/bin/test-*.dll" > $null
    Remove-Item -Force -ErrorAction Ignore "$base/bin/test-*.exe" > $null
    
    # Clear config data
    # Remove-Item build\bin\Release\acts.json

    # Info data
    Copy-Item "LIBRARY.md" "$base/README.md" > $null

    # Compress library
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base-library.zip" > $null
    Write-Host "Packaged to '$base-library.zip'"

    # Remove it from main build
    Remove-Item "$base/bin/acts-common.dll" -ErrorAction Ignore > $null
    Remove-Item "$base/README.md" -ErrorAction Ignore > $null
    Remove-Item "$base/lib" -Recurse -ErrorAction Ignore > $null
    Remove-Item "$base/include" -Recurse -ErrorAction Ignore > $null
    Remove-Item "$base/examples" -Recurse -ErrorAction Ignore > $null

    # Build exec data

    if ($deployQt) {
        $qtActs = Get-Content "qt_acts.json" | ConvertFrom-Json

        $qtversion = windeployqt --version
        Write-Host "Deploy Qt Version $qtversion"

        $qtActs.executables | ForEach-Object {
            windeployqt @($qtActs.options) "$base/bin/$_"
        }
        $qtActs.delete | ForEach-Object {
            Remove-Item "$base/bin/$_" -Recurse -Force -ErrorAction Ignore
        }
    }

    # Info data
    Copy-Item "README.md" "$base/README.md" > $null
    Copy-Item "release/version" "$base/bin/version" > $null

    # Binaries
    Copy-Item "build/bin/Release/*.dll" "$base/bin" > $null
    Copy-Item "build/bin/Release/*.exe" "$base/bin" > $null
    New-Item "$base/bin/package_index" -ItemType Directory > $null
    New-Item "$base/bin/deps" -ItemType Directory > $null
    "Put the game dependencies in this directory." > "$base/bin/deps/README.md"

    # Build hashes
    Write-Host "Building hash index directory"
    foreach ($file in (Get-ChildItem config\common_hashes)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $fileOut = "$base/bin/package_index/$($file.Name.SubString(0, $split)).wni"
        } else {
            $fileOut = "$base/bin/package_index/$($file.Name).wni"
        }

        build\bin\Release\acts.exe --noUpdater -t wni_gen $file.FullName $fileOut
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

        build\bin\Release\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut opaque zstd_hc
    }

    foreach ($file in (Get-ChildItem config\gscbin\functions)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $vmName = $file.Name.SubString(0, $split)
        } else {
            $vmName = $file.Name;
        }
        $fileOut = "$base/bin/package_index/gscbin-functions-$vmName.acef"

        build\bin\Release\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut function zstd_hc
    }
    
    foreach ($file in (Get-ChildItem config\gscbin\methods)) {
        $split = $file.Name.LastIndexOf('.')

        if ($split -ne -1) {
            $vmName = $file.Name.SubString(0, $split)
        } else {
            $vmName = $file.Name;
        }
        $fileOut = "$base/bin/package_index/gscbin-methods-$vmName.acef"

        build\bin\Release\acts.exe --noUpdater -t acef_gscopaque $vmName $file.FullName $fileOut method zstd_hc
    }

    # Compress
    Compress-Archive -LiteralPath "$base" -DestinationPath "$base.zip" > $null
    Write-Host "Packaged to '$base.zip'"

}
finally {
    $prevPwd | Set-Location
}