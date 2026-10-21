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
    $msvc = "v143",
    $clangFormatPath = $null
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    Write-Host "-- Update submodules"
    git submodule update --init --recursive

    $vcpkgCommand = Get-Command vcpkg -CommandType Application -ErrorAction SilentlyContinue |
        Select-Object -First 1
    if ($null -eq $vcpkgCommand) {
        throw "vcpkg was not found on PATH. Install vcpkg and add it to PATH before running setup."
    }

    $env:VCPKG_ROOT = Split-Path -Parent $vcpkgCommand.Path
    $vcpkgToolchain = Join-Path $env:VCPKG_ROOT "scripts/buildsystems/vcpkg.cmake"
    if (-not (Test-Path $vcpkgToolchain)) {
        throw "The vcpkg toolchain was not found at $vcpkgToolchain."
    }

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

    $params += @(
        "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain"
        "-DVCPKG_TARGET_TRIPLET=x64-windows"
    )

    if ($verbose) {
        $params += @( "--log-level", "VERBOSE" )
    }

    if ($fresh) {
        $params += @( "--fresh" )
    }

    if ($null -ne $clangFormatPath) {
        $params += @( "-DCLANG_FORMAT_PATH=$clangFormatPath" )
    }

    cmake -S . -B build -G $build -A x64 -T $msvc @params
}
finally {
    $prevPwd | Set-Location
}