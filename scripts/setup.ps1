param(
    [switch]
    $ci
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Update submodules"
    git submodule update --init --recursive

    Write-Host "-- Install packages"
    vcpkg install "@.\packages.txt"

    #Write-Host "-- Create jit-lua projects"
    #Push-Location deps\jit-lua
    #premake5 vs2022
    #Pop-Location

    Write-Host "-- Create solution"
    if ($ci) {
        premake5 vs2022 --ci-build
    } else {
        premake5 vs2022
    }
}
finally {
    $prevPwd | Set-Location
}