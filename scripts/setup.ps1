param(
    [switch]
    $ci
)



$prevPwd = $PWD

try {
    $base = (Get-Item $PSScriptRoot).parent
    Set-Location ($base.Fullname)
    
    # Update submodules
    git submodule update --init --recursive
    # install packages
    vcpkg install "@.\packages.txt"
    # create solution
    if ($ci) {
        premake5 vs2022 --ci-build
    } else {
        premake5 vs2022
    }
}
finally {
    $prevPwd | Set-Location
}