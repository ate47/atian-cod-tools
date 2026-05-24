param(
    [switch]
    $ci,
    [switch]
    $gpl,
    [switch]
    $fresh,
    [switch]
    $verbose,
    [switch]
    $noQT,
    [switch]
    $noOpenCL,
    [switch]
    $allPlatform,
    $build = "Visual Studio 18 2026",
    $msvc = "v143"
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
    if ($noOpenCL) {
        $params += @( "-DNO_OPEN_CL_BUILD=ON" )
    }
    if ($allPlatform) {
        $params += @( "-DALL_PLATFORM_ACTS=ON" )
    }

    if ($verbose) {
        $params += @( "--log-level", "VERBOSE" )
    }

    if ($fresh) {
        $params += @( "--fresh" )
    }

    cmake -S . -B build -G $build -A x64 -T $msvc @params
}
finally {
    $prevPwd | Set-Location
}