param(
    [switch]
    $ci,
    [switch]
    $gpl,
    [switch]
    $fresh,
    [switch]
    $noQT,
    $build = "Visual Studio 18 2026",
    $msvc = "v143",
    $deployType = "app"
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
    if ($noQT) {
        $params += @( "-DNO_QT_BUILD=ON" )
    }
    if ("library" -eq $deployType) {
        $params += @( "-DSTATIC_ACTS=ON" )
        $params += @( "-DNO_QT_BUILD=ON" )
    }

    cmake -S . -B build -G $build -A x64 -T $msvc @params
}
finally {
    $prevPwd | Set-Location
}