param(
    [switch]
    $ci,
    [switch]
    $gpl
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Update submodules"
    git submodule update --init --recursive

    Write-Host "-- Install packages"
    vcpkg install "@.\packages.txt"

    .\scripts\copydata.ps1

    Write-Host "-- Create solution"
    if ($ci) {
        if ($gpl) {
            premake5 vs2022 --ci-build --gpl-build
        }
        else {
            premake5 vs2022 --ci-build
        }
    }
    else {
        if ($gpl) {
            premake5 vs2022 --gpl-build
        }
        else {
            premake5 vs2022
        }
    }
}
finally {
    $prevPwd | Set-Location
}