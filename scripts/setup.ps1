param(
    [switch]
    $ci,
    [switch]
    $gpl,
    $build = "Visual Studio 18 2026"
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
            cmake -S . -B build -G $build -A x64  "-DCI_BUILD=ON" "-DGPL_BUILD=ON"
        }
        else {
            cmake -S . -B build -G $build -A x64 "-DCI_BUILD=ON"
        }
    }
    else {
        if ($gpl) {
            cmake -S . -B build -G $build -A x64 "-DGPL_BUILD=ON"
        }
        else {
            cmake -S . -B build -G $build -A x64
        }
    }
}
finally {
    $prevPwd | Set-Location
}