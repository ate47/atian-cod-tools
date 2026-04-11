param(
    $outDirectory,
    [switch]
    $deployQt
)

$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)

    # Delete previous builds
    Remove-Item -Recurse -Force -ErrorAction Ignore  "$outDirectory" > $null
    New-Item "$outDirectory" -ItemType Directory > $null

 
    # Binaries
    Copy-Item "build/bin/Release/*.dll" "$outDirectory" -ErrorAction Ignore > $null
    Copy-Item "build/bin/Release/*.exe" "$outDirectory" -ErrorAction Ignore > $null
    Copy-Item "build/bin/Release/acts-common.lib" "$outDirectory" -ErrorAction Ignore > $null
    Copy-Item "build/bin/Release/acts-common-static.lib" "$outDirectory" -ErrorAction Ignore > $null
    Remove-Item -Force -ErrorAction Ignore "$outDirectory/test-*.dll" > $null
    Remove-Item -Force -ErrorAction Ignore "$outDirectory/test-*.exe" > $null

    # Build exec data

    if ($deployQt) {
        $qtActs = Get-Content "qt_acts.json" | ConvertFrom-Json

        $qtversion = windeployqt --version
        Write-Host "Deploy Qt Version $qtversion"

        $qtActs.executables | ForEach-Object {
            windeployqt @($qtActs.options) "$outDirectory/$_"
        }
        $qtActs.delete | ForEach-Object {
            Remove-Item "$outDirectory/$_" -Recurse -Force -ErrorAction Ignore
        }
    }
}
finally {
    $prevPwd | Set-Location
}