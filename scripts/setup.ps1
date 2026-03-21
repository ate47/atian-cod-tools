param(
    [switch]
    $ci,
    [switch]
    $gpl,
    [switch]
    $fresh,
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
    $params = @()

    if ($ci) {
        $params += @( "-DCI_BUILD=ON" )
    }
    if ($gpl) {
        $params += @( "-DGPL_BUILD=ON" )
    }
    if ($fresh) {
        $params += @( "--fresh" )
    }

    cmake -S . -B build -G $build -A x64 @params
}
finally {
    $prevPwd | Set-Location
}